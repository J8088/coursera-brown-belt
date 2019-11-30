#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>
#include <cfloat>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

using namespace std;

typedef int64_t I64;
typedef vector<int> vi;
typedef vector<bool> vb;
typedef pair<int, int> pi;

#define fr(i,a,b) for (int i = a; i <= b; i++)
#define fr2(i,a,b) for (int i = a; i < b; i++)

struct Booking {
	I64 time;
	string hotel;
	int client_id;
	int room_count;
};

class BookingManager {
public:
	void Book(I64 time, const string& hotel, int client_id, int room_count) {
		bookings.push({ time, hotel, client_id, room_count });
		hotels_rooms_count[hotel] += room_count;
		hotels_clients[hotel][client_id] += room_count;
		RotateOld(time);
	}

	I64 Clients(const string& hotel) const {
		if (!hotels_rooms_count.count(hotel)) {
			return 0;
		}

		return hotels_clients.at(hotel).size();
	}

	I64 Rooms(const string& hotel) const {
		if (!hotels_rooms_count.count(hotel)) {
			return 0;
		}

		return hotels_rooms_count.at(hotel);
	}
private:
	queue<Booking> bookings;
	map<string, I64> hotels_rooms_count;
	map<string, map<int, I64>> hotels_clients;

	void RotateOld(I64 time) {
		I64 yesterday = time - 86400;
		while (!bookings.empty() && bookings.front().time <= yesterday) {
			const Booking& booking = bookings.front();

			hotels_rooms_count[booking.hotel] -= booking.room_count;

			hotels_clients[booking.hotel][booking.client_id] -= booking.room_count;
			if (hotels_clients[booking.hotel][booking.client_id] == 0) {
				hotels_clients[booking.hotel].erase(booking.client_id);
			}

			bookings.pop();
		}
	}
};


int main() {
	ios_base::sync_with_stdio(false);

	BookingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		if (query_type == "BOOK") {
			I64 time;
			string hotel;
			int client_id, room_count;
			cin >> time >> hotel >> client_id >> room_count;
			manager.Book(time, hotel, client_id, room_count);
		}
		else if (query_type == "CLIENTS") {
			string hotel;
			cin >> hotel;
			cout << manager.Clients(hotel) << "\n";
		}
		else if (query_type == "ROOMS") {
			string hotel;
			cin >> hotel;
			cout << manager.Rooms(hotel) << "\n";
		}
	}

	return 0;
}
