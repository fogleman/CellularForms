#pragma once

#include <functional>
#include <string>

void Panic(const std::string &message);

std::function<void()> Timed(const std::string &message);

double Random(const double lo, const double hi);

int RandomIntN(const int n);
