//
// Created by pc on 2023/3/7.
//

#include "Table.h"
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

Hand::Hand(const std::string& str) {
    memset(cards, 0, sizeof(cards));
    num = 0;
    for (auto card: str) {
        num++;
        switch (card) {
            case 'A':
                cards[A] ++;
                break;
            case 'K':
                cards[K] ++;
                break;
            case 'Q':
                cards[Q] ++;
                break;
            case 'J':
                cards[J] ++;
                break;
            case 'T':
                cards[T] ++;
                break;
            case '9':
                cards[card_9] ++;
                break;
            case '8':
                cards[card_8] ++;
                break;
            case '7':
                cards[card_7] ++;
                break;
            case '6':
                cards[card_6] ++;
                break;
            case '5':
                cards[card_5] ++;
                break;
            case '4':
                cards[card_4] ++;
                break;
            case '3':
                cards[card_3] ++;
                break;
            case '2':
                cards[card_2] ++;
                break;
            case 'x':
                cards[x] ++;
                break;
            case 'o':
                cards[o] ++;
                num--;
                break;
            default:
                cout << "Unknown card: " << card << endl;
                assert(0);
        }
    }
}

const std::string Hand::CARD_STR {"AKQJT98765432xo"};

string Hand::to_string() const {
    string ret;
    for (int i = 0; i < NUM_CARD_TYPE; i++) {
        for (int j = 0; j < cards[i]; j++) {
            ret += CARD_STR[i];
        }
    }
    return ret;
}

Table::Table(const std::string& n, const std::string& e, const std::string& s, const std::string& w) {
    hands[0] = Hand(n);
    hands[1] = Hand(e);
    hands[2] = Hand(s);
    hands[3] = Hand(w);
}

Table::Table(const Hand &n, const Hand &e, const Hand &s, const Hand &w) {
    hands[0] = n;
    hands[1] = e;
    hands[2] = s;
    hands[3] = w;
}

string Table::to_string() const {
    string ret;
    ret += "N: ";
    ret += hands[0].to_string();
    ret += "\nE: ";
    ret += hands[1].to_string();
    ret += "\nS: ";
    ret += hands[2].to_string();
    ret += "\nW: ";
    ret += hands[3].to_string();
    ret += "\n";
    return ret;
}

uint64_t Hand::encode() const { // o does not participate in encoding process
    uint64_t ret = 0;
    for (int i = 0; i < NUM_CARD_TYPE - 1; i++) {
        ret += cards[i];
        ret <<= 4;
    }
    return ret;
}

vector<uint64_t> Table::encode() const {
    vector<uint64_t> ret;
    for (const auto& hand: hands) {
        ret.push_back(hand.encode());
    }
    return ret;
}

void Hand::play(Card card) {
    if (card == o) {
        return;
    }
    if (cards[card] <= 0) {
        cout << "Error: playing nothing." << endl;
        assert(0);
    }
    cards[card]--;
    num--;
}

void Hand::unplay(Card card) {
    if (card == o) {
        return;
    }
    cards[card]++;
    num++;
}

int Table::declarer_num_winners() {
    vector<uint64_t> key = encode();
    int ret = 0;
    if (results.find(key) != results.end()) {
        return results.at(key);
    }

    if (hands[0].num == 0 && hands[2].num == 0) {
        ret = 0;
        goto end;
    }
    if (hands[1].num == 0 && hands[3].num == 0) {
        ret = (hands[0].num > hands[2].num ? hands[0].num : hands[2].num);
        goto end;
    }


    Card card_played[4];
    if (hands[0].num != 0) {
        int best0 = -1;
        for (int i = 0; i < Hand::NUM_CARD_TYPE - 1; i++) {
            if (hands[0].cards[i] > 0) {
                hands[0].play(Card(i));
                card_played[0] = Card(i);

                int best1 = 20;
                bool flag1 = false;
                for (int j = 0; j < Hand::NUM_CARD_TYPE; ++j) {
                    if ((hands[1].cards[j] > 0 && Card(j) != o) || (!flag1 && Card(j) == o)) {
                        flag1 = true;
                        hands[1].play(Card(j));
                        card_played[1] = Card(j);

                        int best2 = -1;
                        bool flag2 = false;
                        for (int k = 0; k < Hand::NUM_CARD_TYPE; ++k) {
                            if ((hands[2].cards[k] > 0 && Card(k) != o) || (!flag2 && Card(k) == o)) {
                                flag2 = true;
                                hands[2].play(Card(k));
                                card_played[2] = Card(k);

                                int best3 = 20;
                                bool flag3 = false;
                                for (int w = 0; w < Hand::NUM_CARD_TYPE; ++w) {
                                    if ((hands[3].cards[w] > 0 && Card(w) != o) || (!flag3 && Card(w) == o)) {
                                        flag3 = true;
                                        hands[3].play(Card(w));
                                        card_played[3] = Card(w);

                                        int result_other = declarer_num_winners();
                                        if ((card_played[0] < card_played[1] && card_played[0] < card_played[3])
                                            || (card_played[2] < card_played[1] && card_played[2] < card_played[3])) {
                                            result_other += 1;
                                        }
                                        if (result_other < best3) {
                                            best3 = result_other;
                                        }
                                        hands[3].unplay(card_played[3]);
                                    }
                                }
                                if (best3 == 20) {
                                    cout << "Error" << endl;
                                    assert(0);
                                }
                                if (best3 > best2) {
                                    best2 = best3;
                                }
                                hands[2].unplay(card_played[2]);
                            }
                        }
                        if (best2 == -1) {
                            cout << "Error" << endl;
                            assert(0);
                        }
                        if (best2 < best1) {
                            best1 = best2;
                        }
                        hands[1].unplay(card_played[1]);
                    }
                }
                if (best1 == 20) {
                    cout << "Error" << endl;
                    assert(0);
                }
                if (best1 > best0) {
                    best0 = best1;
                }
                hands[0].unplay(card_played[0]);
            }
        }
        if (best0 > ret) {
            ret = best0;
        }
    }
    if (hands[2].num != 0) {
        int best0 = -1;
        for (int i = 0; i < Hand::NUM_CARD_TYPE - 1; i++) {
            if (hands[2].cards[i] > 0) {
                hands[2].play(Card(i));
                card_played[2] = Card(i);

                int best1 = 20;
                bool flag1 = false;
                for (int j = 0; j < Hand::NUM_CARD_TYPE; ++j) {
                    if ((hands[3].cards[j] > 0 && Card(j) != o) || (!flag1 && Card(j) == o)) {
                        flag1 = true;
                        hands[3].play(Card(j));
                        card_played[3] = Card(j);

                        int best2 = -1;
                        bool flag2 = false;
                        for (int k = 0; k < Hand::NUM_CARD_TYPE; ++k) {
                            if ((hands[0].cards[k] > 0 && Card(k) != o) || (!flag2 && Card(k) == o)) {
                                flag2 = true;
                                hands[0].play(Card(k));
                                card_played[0] = Card(k);

                                int best3 = 20;
                                bool flag3 = false;
                                for (int w = 0; w < Hand::NUM_CARD_TYPE; ++w) {
                                    if ((hands[1].cards[w] > 0 && Card(w) != o) || (!flag3 && Card(w) == o)) {
                                        flag3 = true;
                                        hands[1].play(Card(w));
                                        card_played[1] = Card(w);

                                        int result_other = declarer_num_winners();
                                        if ((card_played[0] < card_played[1] && card_played[0] < card_played[3])
                                            || (card_played[2] < card_played[1] && card_played[2] < card_played[3])) {
                                            result_other += 1;
                                        }
                                        if (result_other < best3) {
                                            best3 = result_other;
                                        }
                                        hands[1].unplay(card_played[1]);
                                    }
                                }
                                if (best3 == 20) {
                                    cout << "Error" << endl;
                                    assert(0);
                                }
                                if (best3 > best2) {
                                    best2 = best3;
                                }
                                hands[0].unplay(card_played[0]);
                            }
                        }
                        if (best2 == -1) {
                            cout << "Error" << endl;
                            assert(0);
                        }
                        if (best2 < best1) {
                            best1 = best2;
                        }
                        hands[3].unplay(card_played[3]);
                    }
                }
                if (best1 == 20) {
                    cout << "Error" << endl;
                    assert(0);
                }
                if (best1 > best0) {
                    best0 = best1;
                }
                hands[2].unplay(card_played[2]);
            }
        }
        if (best0 > ret) {
            ret = best0;
        }
    }

end:
    results[key] = ret;
    return ret;
}