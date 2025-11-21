#ifndef VALIDATION_H
#define VALIDATION_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
using namespace std;

int TolkoProstieChisla(const string& prompt, int min = 0, int max = 1000);

double TolkoChisla(const string& prompt, double min = 0.0);

bool askYesNo(const string& question);

#endif#pragma once

