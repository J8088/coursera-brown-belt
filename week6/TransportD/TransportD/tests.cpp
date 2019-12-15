#include "database.h"
#include "request.h"
#include "tests.h"
#include "test_runner.h"
#include <iostream>
#include <sstream>

using namespace std;

void TestJsonLoad() {
	using namespace Json;

	stringstream ss;
	ss << R"({
			"type": "Bus",
			"name": "751",
			"id": 194217464,
			"value": -100,
			"value2": 100.03,
			"value3": -1000.123456,
			"bool1": true,
			"bool2": false
		})";
	Document doc = Load(ss);
	const auto& attrs = doc.GetRoot().AsMap();
	ASSERT_EQUAL(attrs.at("type").AsString(), "Bus");
	ASSERT_EQUAL(attrs.at("name").AsString(), "751");
	ASSERT_EQUAL(attrs.at("id").AsInt(), 194217464);
	ASSERT_EQUAL(attrs.at("value").AsInt(), -100);
	ASSERT_EQUAL(attrs.at("value2").AsDouble(), 100.03);
	ASSERT_EQUAL(attrs.at("value3").AsDouble(), -1000.123456);
	ASSERT_EQUAL(attrs.at("bool1").AsBool(), true);
	ASSERT_EQUAL(attrs.at("bool2").AsBool(), false);
}

void TestPrintJson() {
	using namespace Json;
	stringstream ss;
	ss << R"({
"bool1": true,
"bool2": false,
"id": 194217464,
"name": "751",
"type": "Bus",
"value": -100,
"value2": 100.030000,
"value3": -1000.123456
})";
	stringstream expected(ss.str());
	Document doc = Load(ss);
	stringstream os;
	os.precision(6);
	os << doc.GetRoot();
	ASSERT_EQUAL(os.str(), expected.str());
}

void TestRequestToResponse() {
	using namespace Json;
	stringstream ss;
	ss << R"({
				"base_requests": [
				{
					"type": "Stop",
					"road_distances": {
					"Marushkino": 3900
					},
					"longitude": 37.20829,
					"name": "Tolstopaltsevo",
					"latitude": 55.611087
				},
				{
					"type": "Stop",
					"road_distances": {
					"Rasskazovka": 9900
					},
					"longitude": 37.209755,
					"name": "Marushkino",
					"latitude": 55.595884
				},
				{
					"type": "Bus",
					"name": "256",
					"stops": [
					"Biryulyovo Zapadnoye",
					"Biryusinka",
					"Universam",
					"Biryulyovo Tovarnaya",
					"Biryulyovo Passazhirskaya",
					"Biryulyovo Zapadnoye"
					],
					"is_roundtrip": true
				},
				{
					"type": "Bus",
					"name": "750",
					"stops": [
					"Tolstopaltsevo",
					"Marushkino",
					"Rasskazovka"
					],
					"is_roundtrip": false
				},
				{
					"type": "Stop",
					"road_distances": {},
					"longitude": 37.333324,
					"name": "Rasskazovka",
					"latitude": 55.632761
				},
				{
					"type": "Stop",
					"road_distances": {
					"Rossoshanskaya ulitsa": 7500,
					"Biryusinka": 1800,
					"Universam": 2400
					},
					"longitude": 37.6517,
					"name": "Biryulyovo Zapadnoye",
					"latitude": 55.574371
				},
				{
					"type": "Stop",
					"road_distances": {
					"Universam": 750
					},
					"longitude": 37.64839,
					"name": "Biryusinka",
					"latitude": 55.581065
				},
				{
					"type": "Stop",
					"road_distances": {
					"Rossoshanskaya ulitsa": 5600,
					"Biryulyovo Tovarnaya": 900
					},
					"longitude": 37.645687,
					"name": "Universam",
					"latitude": 55.587655
				},
				{
					"type": "Stop",
					"road_distances": {
					"Biryulyovo Passazhirskaya": 1300
					},
					"longitude": 37.653656,
					"name": "Biryulyovo Tovarnaya",
					"latitude": 55.592028
				},
				{
					"type": "Stop",
					"road_distances": {
					"Biryulyovo Zapadnoye": 1200
					},
					"longitude": 37.659164,
					"name": "Biryulyovo Passazhirskaya",
					"latitude": 55.580999
				},
				{
					"type": "Bus",
					"name": "828",
					"stops": [
					"Biryulyovo Zapadnoye",
					"Universam",
					"Rossoshanskaya ulitsa",
					"Biryulyovo Zapadnoye"
					],
					"is_roundtrip": true
				},
				{
					"type": "Stop",
					"road_distances": {},
					"longitude": 37.605757,
					"name": "Rossoshanskaya ulitsa",
					"latitude": 55.595579
				},
				{
					"type": "Stop",
					"road_distances": {},
					"longitude": 37.603831,
					"name": "Prazhskaya",
					"latitude": 55.611678
				}
				],
				"stat_requests": [
				{
					"type": "Bus",
					"name": "256",
					"id": 1965312327
				},
				{
					"type": "Bus",
					"name": "750",
					"id": 519139350
				},
				{
					"type": "Bus",
					"name": "751",
					"id": 194217464
				},
				{
					"type": "Stop",
					"name": "Samara",
					"id": 746888088
				},
				{
					"type": "Stop",
					"name": "Prazhskaya",
					"id": 65100610
				},
				{
					"type": "Stop",
					"name": "Biryulyovo Zapadnoye",
					"id": 1042838872
				}
				]
			})";
	Json::Document doc = Json::Load(ss);

	Database db;
	const auto input_requests = ReadJsonRequests(Request::Mode::MODIFY, doc);
	ProcessInputRequests(db, input_requests);
	const auto get_requests = ReadJsonRequests(Request::Mode::READ, doc);
	const auto responses = ProcessGetRequests(db, get_requests);
		
	stringstream ans;
	ans << ResponsesToJson(responses);

	const string expected = R"([
{
"route_length": 5950,
"request_id": 1965312327,
"curvature": 1.36124,
"stop_count": 6,
"unique_stop_count": 5
},
{
"route_length": 27600,
"request_id": 519139350,
"curvature": 1.31808,
"stop_count": 5,
"unique_stop_count": 3
},
{
"request_id": 194217464,
"error_message": "not found"
},
{
"request_id": 746888088,
"error_message": "not found"
},
{
"buses": [],
"request_id": 65100610
},
{
"buses": [
"256",
"828"
],
"request_id": 1042838872
}
])";
	ASSERT_EQUAL(ans.str(), expected);
}

void RunAllTests() {
	TestRunner tr;
	RUN_TEST(tr, TestJsonLoad);
	RUN_TEST(tr, TestPrintJson);
	RUN_TEST(tr, TestRequestToResponse);
}
