/*
 * recordEntry.cpp
 *
 *  Created on: Sep 30, 2012
 *      Author: agata
 */

#include <cstdlib>
#include <string>
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <limits.h>
#include <stdint.h>

#include "ns3/ptr.h"
#include "ovnis-constants.h"
#include "recordEntry.h"
#include "applications/trafficInformationSystem.h"

using namespace std;

namespace ovnis {

RecordEntry::RecordEntry() {
	reset();
}

void RecordEntry::reset() {
	for (int i = 0; i < LOCAL_MEMORY_SIZE; ++i) {
		senders[i] = "";
		times[i] = 0;
		values[i] = 0;
	}
	count = 0;
	alfa = TIS::getInstance().getDecayFactor();
	decayValue = -1.0;
	decayTime = 0.0;
}


double RecordEntry::getAlfa() {
	return alfa;
}
void RecordEntry::setAlfa(double a) {
	alfa = a;
}

void RecordEntry::add(long packetId, string senderId, double time, double value) {
	// add only newer or first!
//	if (getLatestTime() < time || count < 1) {
	if (true) {
		times[count%LOCAL_MEMORY_SIZE] = time;
		if (value == -1) {
			cerr << "what is this? " << packetId << " " << senderId << " " << time << " " << value << endl;
			value = values[(count-1)%LOCAL_MEMORY_SIZE];
		}
		values[count%LOCAL_MEMORY_SIZE] = value;
		senders[count%LOCAL_MEMORY_SIZE] = senderId;
		packetIds[count%LOCAL_MEMORY_SIZE] = packetId;
		if (time > 0) {
			++count;
		}
	}

	if (decayValue < 0) {
		decayValue = value;
		decayTime = time;
	}
	else if (time > decayTime) { // newer than the last timestamp in db 0.8historical + 0.2received
		decayValue = alfa*decayValue + (1-alfa)*value;
		decayTime = time;
	}
//	else if (time < decayTime ){ // older than existing value in db
//		decayValue = (1-alfa)*decayValue + alfa*value;
//	}
}

void RecordEntry::printValues() {
	cout << "[";
	for (int i = 0; i < LOCAL_MEMORY_SIZE; ++i) {
		cout << times[i] << "," << values[i] << "," << senders[i] << " ";
	}
	cout << "]";
}


double RecordEntry::getValue() {
	string estimationMethod = TIS::getInstance().getTimeEstimationMethod();
	double value = 0;
	if (estimationMethod == "last") {
		value= getLatestValue();
	}
	if (estimationMethod == "decay") {
		value = getDecayValue();
	}
	if (estimationMethod == "average") {
		value = getAverageValue();
	}
	return value;
}

double RecordEntry::getTime() {
	string estimationMethod = TIS::getInstance().getTimeEstimationMethod();
	double value = 0;
	if (estimationMethod == "last") {
		value= getLatestTime();
	}
	if (estimationMethod == "decay") {
		value = getDecayTime();
	}
	if (estimationMethod == "average") {
		value = getAverageTime();
	}
	return value;
}

double RecordEntry::getDecayTime() {
	return decayTime;
}

double RecordEntry::getValue(string estimationMethod) {
	if (estimationMethod == "last") {
		return getLatestValue();
	}
	if (estimationMethod == "decay") {
		return getDecayValue();
	}
	if (estimationMethod == "average") {
		return getAverageValue();
	}
}


double RecordEntry::getDecayValue() {
	return decayValue;
}

double RecordEntry::getLatestValue() {
	return values[(count-1+LOCAL_MEMORY_SIZE)%LOCAL_MEMORY_SIZE];
}

double RecordEntry::getExpectedValue() {
	return expectedValue;
}

double RecordEntry::getActualCapacity() {
	return actualCapacity;
}

double RecordEntry::setCapacity(double expectedValue) {
	this->expectedValue = expectedValue;
	actualCapacity = expectedValue / getLatestValue(); // expected value < latestValue
	return actualCapacity;
}

double RecordEntry::getAverageValue() {
	double sum = 0;
	int num = 0;
	for (int i = 0; i < LOCAL_MEMORY_SIZE; ++i) {
		if (values[i] != 0) {
			sum += values[i];
			++num;
		}
	}
	return sum/num;
}

double RecordEntry::getAverageTime() {
	double sum = 0;
	int num = 0;
	for (int i = 0; i < LOCAL_MEMORY_SIZE; ++i) {
		if (values[i] != 0) {
			sum += times[i];
			++num;
		}
	}
	return sum/num;
}

double RecordEntry::getLatestTime() {
	return times[(count-1+LOCAL_MEMORY_SIZE)%LOCAL_MEMORY_SIZE];
}

string RecordEntry::getLatestSenderId() {
	return senders[(count-1+LOCAL_MEMORY_SIZE)%LOCAL_MEMORY_SIZE];
}

long RecordEntry::getLatestPacketId() {
	return packetIds[(count-1+LOCAL_MEMORY_SIZE)%LOCAL_MEMORY_SIZE];
}

//double RecordEntry::computeAverageValue() {
//	double sum = 0;
//	double count = 0;
//	for (int i = 0; i < LOCAL_MEMORY_SIZE; ++i) {
//		sum += values[i];
//		if (values[i] != 0) {
//			++count;
//		}
//	}
//	return sum/count;
//}

RecordEntry::~RecordEntry() {
}

} /* namespace ovnis */
