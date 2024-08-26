/*
Lecture 1: Interval Scheduling
------------------------------
Topics covered:
- Greedy algorithms
- Dynamic programming
author: Rick Su*/
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
using namespace std;
//to restore the beginning & ending time
class Request{
    public:
        int start;
        int finish;
    Request(int start, int finish) : start(start),finish(finish){}

    bool is_compatible(const Request& req) const{
        return this->finish <=req.start || this->start >= req.finish;
    }
};

vector<Request> greedy_largest_compatible_subset(vector<Request>& requests) {
    vector<Request> result;
    int i = 0;
    sort(requests.begin(), requests.end(), [](const Request& r1, const Request& r2) {
        return r1.finish < r2.finish;
    });

    int n = requests.size();
    while (i < n) {
        Request best = requests[i];
        result.push_back(best);
        i++;
        while (i < n && !requests[i].is_compatible(best)) {
            i++;
        }
    }
    return result;
}

class WeightedRequest : public Request {
public:
    int weight;

    WeightedRequest(int start, int finish, int weight) 
        : Request(start, finish), weight(weight) {}
};

pair<int, vector<WeightedRequest>> optimal_compatible_subset(vector<WeightedRequest>& requests) {
    unordered_map<int, int> dp_weights;
    unordered_map<int, vector<WeightedRequest>> dp_subsets;
    set<int> finishes;

    for (auto& r : requests) {
        finishes.insert(r.finish);
    }

    while (!finishes.empty()) {
        int x = *finishes.rbegin();
        finishes.erase(x);

        unordered_map<int, vector<WeightedRequest>> subsets;
        for (auto& req : requests) {
            if (req.start >= x) {
                int w = req.weight + dp_weights[req.finish];
                subsets[w] = { req };
                subsets[w].insert(subsets[w].end(), dp_subsets[req.finish].begin(), dp_subsets[req.finish].end());
            }
        }

        if (subsets.empty()) {
            dp_weights[x] = 0;
            dp_subsets[x] = {};
        } else {
            int w = max_element(subsets.begin(), subsets.end(), [](const auto& p1, const auto& p2) {
                return p1.first < p2.first;
            })->first;

            dp_weights[x] = w;
            dp_subsets[x] = subsets[w];
        }
    }

    for (auto& req : requests) {
        if (req.start < min_element(dp_weights.begin(), dp_weights.end(), [](const auto& p1, const auto& p2) {
            return p1.first < p2.first;
        })->first) {
            int w = req.weight + dp_weights[req.finish];
            if (dp_weights.find(0) == dp_weights.end() || w > dp_weights[0]) {
                dp_weights[0] = w;
                dp_subsets[0] = { req };
                dp_subsets[0].insert(dp_subsets[0].end(), dp_subsets[req.finish].begin(), dp_subsets[req.finish].end());
            }
        }
    }

    return { dp_weights[0], dp_subsets[0] };
}

int main() {
    // Example usage for greedy_largest_compatible_subset
    vector<Request> requests = { {0, 3}, {2, 5}, {4, 6}, {6, 7}, {5, 8}, {8, 9} };
    vector<Request> result = greedy_largest_compatible_subset(requests);

    cout << "Greedy largest compatible subset:" << endl;
    for (auto& r : result) {
        cout << "Request(" << r.start << ", " << r.finish << ")" << endl;
    }

    // Example usage for optimal_compatible_subset
    vector<WeightedRequest> weightedRequests = { {0, 3, 4}, {2, 5, 2}, {4, 6, 4}, {6, 7, 7}, {5, 8, 2}, {8, 9, 1} };
    auto [maxWeight, optimalSubset] = optimal_compatible_subset(weightedRequests);

    cout << "Optimal weighted compatible subset:" << endl;
    for (auto& r : optimalSubset) {
        cout << "WeightedRequest(" << r.start << ", " << r.finish << ", " << r.weight << ")" << endl;
    }
    cout << "Max Weight: " << maxWeight << endl;

    return 0;
}
