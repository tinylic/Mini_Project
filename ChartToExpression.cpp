/*
 * ChartToExpression.cpp
 *
 *  Created on: 2016年3月3日
 *      Author: tinylic
 */

#include "ChartToExpression.h"

ChartToExpression::ChartToExpression() {
	// TODO Auto-generated constructor stub

}

ChartToExpression::~ChartToExpression() {
	// TODO Auto-generated destructor stub
}

int ChartToExpression::CountOne(int x) {
	int ans = 0;
	while (x) {
		if (x & 1)
			ans++;
		x >>= 1;
	}
	return ans;
}

bool ChartToExpression::CheckContained(const implication &imp, const int &x) {
	int t = x;
	for (int i = 0; i < imp.TotalVariables; i++) {
		int value = t & 1;
		t >>= 1;
		if (imp.exp[i] == '-')
			continue;
		if (value != (imp.exp[i] - '0'))
			return false;
	}
	return true;
}

void ChartToExpression::Simplify() {
	for (unsigned i = 0; i < MinTerm.size(); i++)
		if (MinTermCovered[i].size() == 1) {
			int index = MinTermCovered[i][0];
			primes[index].selected = true;
			for (unsigned j = 0; j < primes[index].ImpContained.size(); j++)
				contained[primes[index].ImpContained[j]] = true;
		}

	//petrick
	UPI.clear();
	for (unsigned i = 0; i < primes.size(); i++)
		if (primes[i].selected == false)
			UPI.push_back(&primes[i]);
	bool tContained[1 << MAX_N];
	int MinOne = 1 << MAX_N;
	int result = 0;
	for (int mask = 0; mask < (1 << (UPI.size())); mask++) {
		memset(tContained, false, sizeof tContained);
		for (unsigned i = 0; i < UPI.size(); i++) {
			int value = (mask >> i) & 1;
			implication *mImp = UPI[i];
			if (value) {
				for (unsigned j = 0; j < mImp->ImpContained.size(); j++)
					tContained[mImp->ImpContained[j]] = true;
			}
		}
		bool valid = true;
		for (unsigned i = 0; i < MinTerm.size(); i++)
			if (contained[i] == false && tContained[i] == false)
				valid = false;
		if (valid) {
			int ones = CountOne(mask);
			if (MinOne > ones) {
				MinOne = ones;
				result = mask;
			}
		}
	}
	for (int i = 0; i < UPI.size(); i++) {
		int value = (result >> i) & 1;
		if (value)
			UPI[i]->selected = true;
	}
}

void ChartToExpression::solve() {
	memset(table, false, sizeof table);
	memset(contained, false, sizeof contained);
	imp.clear();
	int x;
	while ((scanf("%d", &x)) && x != -1) {
		MinTerm.push_back(x);
	}
	sort(MinTerm.begin(), MinTerm.end());
	for (; (1 << TotalVariables) <= MinTerm[MinTerm.size() - 1];
			TotalVariables++)
		;
	cout << TotalVariables << endl;
	for (int i = 0; i < MinTerm.size(); i++)
		imp.push_back(implication(MinTerm[i], 0, TotalVariables));
	sort(imp.begin(), imp.end());
	while (imp.size() > 0) {
		roller.clear();
		for (int i = 0; i < imp.size(); i++)
			for (int j = i + 1; j < imp.size(); j++)
				if (imp[i].exp == imp[j].exp) {
					imp.erase(imp.begin() + j);
				}
		for (int i = 0; i < imp.size(); i++)
			for (int j = i + 1; j < imp.size(); j++)
				if (imp[j].ones == imp[i].ones + 1
						&& imp[j].xterm == imp[i].xterm
						&& CountOne(imp[j].bit ^ imp[i].bit) == 1) {
					imp[i].used = true;
					imp[j].used = true;
					implication NewImp = implication(imp[i].bit,
							(imp[i].bit ^ imp[j].bit) | imp[i].xterm,
							TotalVariables);
					roller.push_back(NewImp);
				}
		for (int i = 0; i < imp.size(); i++)
			if (imp[i].used == false)
				primes.push_back(imp[i]);
		sort(roller.begin(), roller.end());
		imp = roller;
		cout << "---------------------------" << endl;
		for (int i = 0; i < imp.size(); i++)
			cout << imp[i].ones << "\t" << imp[i].exp << "\t"
					<< (imp[i].used ? 'X' : ' ') << endl;
	}
	sort(primes.begin(), primes.end());
	for (int i = 0; i < MinTerm.size(); i++)
		MinTermCovered[i].clear();
	for (int i = 0; i < primes.size(); i++)
		for (int j = 0; j < MinTerm.size(); j++)
			if (CheckContained(primes[i], MinTerm[j])) {
				table[i][j] = true;
				primes[i].ImpContained.push_back(j);
				MinTermCovered[j].push_back(i);
			}
	for (int i = 0; i < MinTerm.size(); i++)
		cout << "  \t" << MinTerm[i];
	cout << endl;
	for (int i = 0; i < primes.size(); i++) {
		cout << primes[i].exp;
		for (int j = 0; j < MinTerm.size(); j++) {
			cout << "  \t";
			if (table[i][j])
				cout << "X";
		}
		cout << endl;
	}
	Simplify();
	bool head = true;
	for (int i = 0; i < primes.size(); i++)
		if (primes[i].selected) {
			if (!head)
				cout << " + ";
			cout << primes[i].show();
			head = false;
		}
}