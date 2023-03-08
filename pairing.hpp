#pragma once
#include <array>
#include <vector>

class fp;
class fp2;
class fp6;
class fp12;
class g1;
class g2;

class pairing
{

public:
    static void doublingStep(array<fp2, 3>& coeff, g2& r);
    static void additionStep(array<fp2, 3>& coeff, g2& r, g2& tp);
    static void preCompute(array<array<fp2, 3>, 68>& ellCoeffs, g2& twistPoint);
    static fp12 millerLoop(vector<tuple<g1, g2>>& pairs);
    static void finalExp(fp12& f);
    static fp12 calculate(vector<tuple<g1, g2>>& pairs);
    static void addPair(vector<tuple<g1, g2>>& pairs, const g1& e1, const g2& e2);
};