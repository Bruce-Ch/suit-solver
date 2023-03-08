//
// Created by pc on 2023/3/7.
//

#ifndef SUIT_SOLVER_TABLE_H
#define SUIT_SOLVER_TABLE_H

#include <string>
#include <map>
#include <vector>

enum Card {
    A, K, Q, J, T, card_9, card_8, card_7, card_6, card_5, card_4, card_3, card_2, x, o
};

class Hand {
public:
    const static int NUM_CARD_TYPE = 15;
    const static std::string CARD_STR;
    int cards[NUM_CARD_TYPE]{};
    int num{};

public:
    Hand(): num(0) {}
    explicit Hand(const std::string& str);
    ~Hand() = default;
    std::string to_string() const;
    uint64_t encode() const;
    void play(Card card);
    void unplay(Card card);
};

class Table {
    Hand hands[4]; // N E S W
    std::map<std::vector<uint64_t>, int> results;
public:
    Table(const Hand& n, const Hand& e, const Hand& s, const Hand& w);
    Table(const std::string& n, const std::string& e, const std::string& s, const std::string& w);
    ~Table() = default;
    std::string to_string() const;

    int declarer_num_winners();
    std::vector<uint64_t> encode() const;
};


#endif //SUIT_SOLVER_TABLE_H
