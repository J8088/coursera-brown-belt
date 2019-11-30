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

enum class TaskStatus {
	NEW,
	IN_PROGRESS,
	TESTING,
	DONE
};

using TasksInfo = map<TaskStatus, int>;

TaskStatus NextStatus(TaskStatus status) {
	switch (status)
	{
	case TaskStatus::NEW:
		return TaskStatus::IN_PROGRESS;
	case TaskStatus::IN_PROGRESS:
		return TaskStatus::TESTING;
	case TaskStatus::TESTING:
		return TaskStatus::DONE;
	default:
		return TaskStatus::DONE;
	}
}

ostream& operator<<(ostream& out, const TaskStatus& status) {
	switch (status)
	{
	case TaskStatus::NEW:
		return out << "NEW";
	case TaskStatus::IN_PROGRESS:
		return out << "IN_PROGRESS";
	case TaskStatus::TESTING:
		return out << "TESTING";
	default:
		return out << "DONE";
	}
}

ostream& operator<<(ostream& out, const TasksInfo& tasks) {
	out << '{';
	bool first = true;
	for (const auto& [status, count] : tasks) {
		if (!first) {
			out << ", ";
		}
		first = false;
		out << '"' << status << "\": " << count;
	}
	out << '}';

	return out;
}

ostream& operator<<(ostream& out, tuple<TasksInfo, TasksInfo> t) {
	return out << '[' << get<0>(t) << ", " << get<1>(t) << ']';
}

class TeamTasks {
public:
	const TasksInfo& GetPersonTasksInfo(const string& person) const {
		return tasks.at(person);
	}

	void AddNewTask(const string& person) {
		tasks[person][TaskStatus::NEW]++;
	}

	tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
		TasksInfo updated_tasks = tasks[person];
		TasksInfo old_tasks, new_tasks;

		for (auto status : { TaskStatus::NEW, TaskStatus::IN_PROGRESS, TaskStatus::TESTING }) {
			TaskStatus next_status = NextStatus(status);
			if (tasks[person].count(status)) {
				int cur_count = tasks[person][status];
				if (task_count == 0) {
					old_tasks[status] = cur_count;
				}
				else if (cur_count <= task_count) {
					updated_tasks[status] -= cur_count;
					if (updated_tasks[status] == 0) {
						updated_tasks.erase(status);
					}
					new_tasks[next_status] = cur_count;
					updated_tasks[next_status] += cur_count;
					task_count -= cur_count;
				}
				else {
					old_tasks[status] = cur_count - task_count;
					updated_tasks[status] -= task_count;
					if (updated_tasks[status] == 0) {
						updated_tasks.erase(status);
					}
					updated_tasks[next_status] += task_count;
					new_tasks[next_status] = task_count;
					task_count = 0;
				}
			}
		}
		tasks[person] = updated_tasks;

		return tie(new_tasks, old_tasks);
	}
private:
	map<string, TasksInfo> tasks;
};

int main() {
	TeamTasks tasks;

	string line;
	while (getline(cin, line)) {
		stringstream ss(line);
		string cmd;
		ss >> cmd;
		if (cmd == "AddNewTasks") {
			string person;
			int count;
			ss >> person >> count;

			fr2(i, 0, count) tasks.AddNewTask(person);
		}
		else if (cmd == "PerformPersonTasks") {
			string person;
			int count;
			ss >> person >> count;

			cout << tasks.PerformPersonTasks(person, count) << endl;
		}
		else if (cmd == "GetPersonTasksInfo") {
			string person;
			ss >> person;

			cout << tasks.GetPersonTasksInfo(person) << endl;
		}
	}

	return 0;
}
