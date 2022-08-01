#ifndef ADMIN_H_
#define ADMIN_H_

#include "citizen.cpp"
#include "dateutil.cpp"
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

class Admin
{
	string pswd;
	static int available_shots; // The number of vaccine shots available per day
	static string last_date;	// Used by the run_process function of this class to record the date of its last execution

public:
	static int age_eligibility; // The minimum age eligible for registration

	Admin()
	{
		pswd = "vaccine";
	}
	bool run_process(CitizenQueueList &cqueue);
	void display_top_n(CitizenQueueList &cqueue);
	void display_today(CitizenQueueList &cqueue);
	void change_eligibility(CitizenQueueList &cqueue);
	void change_available_shots();
	bool verify_password(string pass);
	void remove(Citizen *user, string pswd, CitizenQueueList &cqueue);
	void remove_age_ineligible(CitizenQueueList &cqueue);

	friend class CitizenQueueList;
};

int Admin::age_eligibility = 18;
int Admin::available_shots = 1;
string Admin::last_date = "";

bool Admin::verify_password(string pass)
{
	return pass == pswd;
}

bool Admin::run_process(CitizenQueueList &cqueue)
{
	Citizen *selected = cqueue.front;
	Citizen *dq = nullptr;

	// Extracting today's date
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int day = ltm->tm_mday;
	int month = ltm->tm_mon + 1;
	int year = ltm->tm_year + 1900;


	string today = date_string(day, month, year); 

	if (today == last_date)
		return false;

	// ALLOTMENT PROCESS:

	// For citizens who have been allocated slots previously
	while (selected != nullptr && selected->visible)
	{
		if (selected->slot_date == today) // Citizens whose slot is today
		{
			selected->slot_today = true;  // Modifies the citizen's slot_today member
			selected = selected->next;
		}
		else
		{
			

			selected = selected->next; // Preserving the traversal before moving the element

			int new_priority = cqueue.get_min_priority() - 1;
			dq = cqueue.dequeue();
			dq->visible = false;
			dq->slot_today = false;
			dq->demoted = true;
			dq->priority = new_priority;
			dq->slot_date = "Not Alloted Yet.";

			cqueue.enqueue(dq);
		}

	}
	string dslot = get_next_date(day, month, year); // Generates the string representation for the following day.

	for (int count = 1; count <= available_shots; count++)
	{
		if (selected == nullptr)
			break;
		selected->slot_date = dslot;
		selected->visible = true;
		selected = selected->next;
	}

	last_date = today; // Modifies the last_date member to today's date

	return true;
}

void Admin::display_top_n(CitizenQueueList &cqueue)
{
	unsigned n;
	cout << "\n\tHow many entries are to be displayed ?";
	cin >> n;
	cqueue.display_top_n(n);
}

void Admin::display_today(CitizenQueueList &cqueue)
{
	cqueue.display_today();
}
void Admin::change_eligibility(CitizenQueueList &cqueue)
{
	
	cout << "\n\tEnter eligibility age : ";
	cin >> age_eligibility;
	remove_age_ineligible(cqueue);
	cout << "\n\t\tChanged successfully." << endl;
}

void Admin::change_available_shots()
{
	/*
	 * Allows admin to change available_shots as per actual availability
	 */
	cout << "\n\tEnter new number : ";
	cin >> available_shots;
	cout << "Changed successfully." << endl;
}

void Admin::remove(Citizen *user, string pswd, CitizenQueueList &cqueue)
{
	cqueue.remove(user, pswd);
}

void Admin::remove_age_ineligible(CitizenQueueList& cqueue)
{
	Citizen* cur = cqueue.front;
	Citizen* prev = nullptr;
	Citizen* to_delete = nullptr;
	bool delete_flag = false;

	while(cur != nullptr){
		delete_flag = cur->age < age_eligibility && !cur->visible;
		if (delete_flag)
		{
			if (prev == nullptr) // Removing from the front
				cqueue.front = cur->next;
			else // Removing from other positions
				prev->next = cur->next;
			if (cqueue.rear == cur) // Removing from rear
				cqueue.rear = prev;
			to_delete = cur;
			cur = cur->next;
			to_delete->next = nullptr;
			delete(to_delete);
		}
		else
		{
			prev = cur; // change prev only if cur is not to be deleted
			cur = cur->next;
		}

	}
}

#endif 
