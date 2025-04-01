#include <cstring>
#include "../Main/employee.h"

#ifndef EMPLOYEEREPORT_H
#define EMPLOYEEREPORT_H

struct employeeReport {
    int num;
    char name[10];
    double hours;
    double salary;

    employeeReport(const employee& emp, double hourlyRate) {
        num = emp.num;
        strcpy(name, emp.name);
        hours = emp.hours;
        salary = hours * hourlyRate;
    }

    bool operator < (const employeeReport& other) const {
        return num < other.num;
    }
};

#endif