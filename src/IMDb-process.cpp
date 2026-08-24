#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <random>
#include <vector>
#include <algorithm>

bool is_leap(int year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    if (year % 400 != 0) return false;
    return true;
}

long long get_year_start(int year) {
    static std::map<int, long long> cache;
    auto it = cache.find(year);
    if (it != cache.end()) {
        return it->second;
    }

    if (year == 1970) {
        cache[year] = 0;
        return 0;
    }

    int start_year = std::min(year, 1970);
    int end_year = std::max(year, 1970);
    long long total_days = 0;
    for (int y = start_year; y < end_year; y++) {
        total_days += is_leap(y) ? 366 : 365;
    }
    if (year < 1970) {
        total_days = -total_days;
    }
    long long seconds = total_days * 86400LL;
    cache[year] = seconds;
    return seconds;
}

int main() {
    std::map<std::string, std::pair<std::string, int>> basics_map;
    std::map<std::string, std::pair<double, int>> ratings_map;

    std::ifstream basics_file("IMDb-title.basics.tsv");
    if (!basics_file) {
        std::cerr << "Failed to open title.basics.tsv" << std::endl;
        return 1;
    }

    std::string line;
    std::getline(basics_file, line);

    while (std::getline(basics_file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(iss, token, '\t')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 6) {
            continue;
        }

        std::string tconst = tokens[0];
        std::string primaryTitle = tokens[2];
        std::string startYearStr = tokens[5];

        if (startYearStr == "\\N") {
            continue;
        }

        try {
            int startYear = std::stoi(startYearStr);
            basics_map[tconst] = std::make_pair(primaryTitle, startYear);
        } catch (...) {
            continue;
        }
    }

    std::ifstream ratings_file("IMDb-title.ratings.tsv");
    if (!ratings_file) {
        std::cerr << "Failed to open title.ratings.tsv" << std::endl;
        return 1;
    }

    std::getline(ratings_file, line);
    while (std::getline(ratings_file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(iss, token, '\t')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 3) {
            continue;
        }

        std::string tconst = tokens[0];
        try {
            double averageRating = std::stod(tokens[1]);
            int numVotes = std::stoi(tokens[2]);
            ratings_map[tconst] = std::make_pair(averageRating, numVotes);
        } catch (...) {
            continue;
        }
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::ofstream out("IMDb-processed.txt");

    out << "tconst\tprimaryTitle\treleaseTimestamp\taverageRating\tnumVotes\n";

    for (const auto& entry : basics_map) {
        const std::string& tconst = entry.first;
        const std::string& primaryTitle = entry.second.first;
        int startYear = entry.second.second;

        auto ratings_it = ratings_map.find(tconst);
        if (ratings_it == ratings_map.end()) {
            continue;
        }

        double averageRating = ratings_it->second.first;
        int numVotes = ratings_it->second.second;

        long long start_timestamp = get_year_start(startYear);
        long long end_timestamp = get_year_start(startYear + 1);
        long long duration = end_timestamp - start_timestamp;

        long long random_timestamp = start_timestamp;
        if (duration > 0) {
            std::uniform_int_distribution<long long> dist(0, duration - 1);
            random_timestamp = start_timestamp + dist(gen);
        }

        out << tconst << "\t" << primaryTitle << "\t" << random_timestamp << "\t" << averageRating << "\t" << numVotes << "\n";
    }

    out.close();

    return 0;
}