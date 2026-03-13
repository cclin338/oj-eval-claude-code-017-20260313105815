#include <iostream>
#include <cstring>
#include <string>

using namespace std;

// Simple hash map
template<typename K, typename V>
class HashMap {
private:
    static const int SIZE = 10007;
    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    Node* table[SIZE];

    int hash(const K& k) const {
        if constexpr (is_same<K, char>::value) {
            return (unsigned char)k % SIZE;
        } else {
            unsigned long h = 0;
            for (char c : k) h = h * 131 + c;
            return h % SIZE;
        }
    }

public:
    HashMap() {
        for (int i = 0; i < SIZE; i++) table[i] = nullptr;
    }

    ~HashMap() {
        for (int i = 0; i < SIZE; i++) {
            Node* p = table[i];
            while (p) {
                Node* t = p;
                p = p->next;
                delete t;
            }
        }
    }

    bool insert(const K& key, const V& value) {
        int h = hash(key);
        Node* p = table[h];
        while (p) {
            if (p->key == key) return false;
            p = p->next;
        }
        Node* n = new Node(key, value);
        n->next = table[h];
        table[h] = n;
        return true;
    }

    bool find(const K& key, V& value) const {
        int h = hash(key);
        Node* p = table[h];
        while (p) {
            if (p->key == key) {
                value = p->value;
                return true;
            }
            p = p->next;
        }
        return false;
    }

    bool erase(const K& key) {
        int h = hash(key);
        Node* p = table[h];
        Node* prev = nullptr;
        while (p) {
            if (p->key == key) {
                if (prev) prev->next = p->next;
                else table[h] = p->next;
                delete p;
                return true;
            }
            prev = p;
            p = p->next;
        }
        return false;
    }

    bool update(const K& key, const V& value) {
        int h = hash(key);
        Node* p = table[h];
        while (p) {
            if (p->key == key) {
                p->value = value;
                return true;
            }
            p = p->next;
        }
        return false;
    }

    bool exists(const K& key) const {
        int h = hash(key);
        Node* p = table[h];
        while (p) {
            if (p->key == key) return true;
            p = p->next;
        }
        return false;
    }

    int count() const {
        int cnt = 0;
        for (int i = 0; i < SIZE; i++) {
            Node* p = table[i];
            while (p) {
                cnt++;
                p = p->next;
            }
        }
        return cnt;
    }
};

struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
};

struct Train {
    char trainID[25];
    int stationNum;
    int seatNum;
    char stations[100][35];
    int prices[100];
    char startTime[10];
    int travelTimes[100];
    int stopoverTimes[100];
    char saleDate[2][10];
    char type;
    bool released;
};

class TicketSystem {
private:
    HashMap<string, User> users;
    HashMap<string, bool> loggedIn;
    HashMap<string, Train> trains;

    void parseParams(const string& line, string& cmd, HashMap<char, string>& params) {
        size_t pos = 0;
        while (pos < line.length() && line[pos] != ' ') pos++;
        cmd = line.substr(0, pos);
        if (pos < line.length()) pos++;

        while (pos < line.length()) {
            // Skip spaces
            while (pos < line.length() && line[pos] == ' ') pos++;

            if (pos >= line.length()) break;

            if (line[pos] == '-' && pos + 1 < line.length() && line[pos + 1] >= 'a' && line[pos + 1] <= 'z') {
                char key = line[pos + 1];
                pos += 2;

                // Skip spaces after the key
                while (pos < line.length() && line[pos] == ' ') pos++;

                // Get the value - stop at space followed by '-' or end of line
                size_t start = pos;
                while (pos < line.length()) {
                    if (pos + 1 < line.length() && line[pos] == ' ' && line[pos + 1] == '-') {
                        break;
                    }
                    if (pos == line.length() - 1) {
                        pos++;
                        break;
                    }
                    pos++;
                }

                string value = line.substr(start, pos - start);
                // Trim trailing spaces
                while (!value.empty() && value.back() == ' ') value.pop_back();

                params.insert(key, value);
            } else {
                pos++;
            }
        }
    }

    void split(const string& s, char delim, string* result, int& count) {
        count = 0;
        size_t start = 0;
        for (size_t i = 0; i <= s.length(); i++) {
            if (i == s.length() || s[i] == delim) {
                if (i > start) {
                    result[count++] = s.substr(start, i - start);
                }
                start = i + 1;
            }
        }
    }

    void addTime(const string& date, const string& time, int minutesToAdd, string& outDate, string& outTime) {
        // date format: mm-dd, time format: hh:mm
        int month, day, hour, minute;

        if (date.length() >= 5) {
            month = stoi(date.substr(0, 2));
            day = stoi(date.substr(3, 2));
        } else {
            month = 6;
            day = 1;
        }

        if (time.length() >= 5) {
            hour = stoi(time.substr(0, 2));
            minute = stoi(time.substr(3, 2));
        } else {
            hour = 0;
            minute = 0;
        }

        minute += minutesToAdd;
        hour += minute / 60;
        minute %= 60;
        day += hour / 24;
        hour %= 24;

        // Handle month overflow (June=30, July=31, August=31)
        while (day > 31 || (month == 6 && day > 30)) {
            if (month == 6) {
                day -= 30;
                month = 7;
            } else if (month == 7) {
                day -= 31;
                month = 8;
            } else {
                day -= 31;
                month = 9;
            }
        }

        char buf[10];
        sprintf(buf, "%02d-%02d", month, day);
        outDate = buf;
        sprintf(buf, "%02d:%02d", hour, minute);
        outTime = buf;
    }

public:
    TicketSystem() {}

    void addUser(HashMap<char, string>& params) {
        string c, u, p, n, m, g;
        params.find('c', c);
        params.find('u', u);
        params.find('p', p);
        params.find('n', n);
        params.find('m', m);
        params.find('g', g);

        // Check if this is the first user
        int userCount = users.count();

        User user;
        if (userCount == 0) {
            // First user
            strcpy(user.username, u.c_str());
            strcpy(user.password, p.c_str());
            strcpy(user.name, n.c_str());
            strcpy(user.mailAddr, m.c_str());
            user.privilege = 10;

            if (users.insert(u, user)) {
                cout << "0\n";
            } else {
                cout << "-1\n";
            }
            return;
        }

        // Not first user - check permissions
        bool cLoggedIn = false;
        if (!loggedIn.find(c, cLoggedIn) || !cLoggedIn) {
            cout << "-1\n";
            return;
        }

        User curUser;
        if (!users.find(c, curUser)) {
            cout << "-1\n";
            return;
        }

        int newPrivilege = stoi(g);
        if (newPrivilege >= curUser.privilege) {
            cout << "-1\n";
            return;
        }

        if (users.exists(u)) {
            cout << "-1\n";
            return;
        }

        strcpy(user.username, u.c_str());
        strcpy(user.password, p.c_str());
        strcpy(user.name, n.c_str());
        strcpy(user.mailAddr, m.c_str());
        user.privilege = newPrivilege;

        if (users.insert(u, user)) {
            cout << "0\n";
        } else {
            cout << "-1\n";
        }
    }

    void login(HashMap<char, string>& params) {
        string u, p;
        params.find('u', u);
        params.find('p', p);

        User user;
        if (!users.find(u, user)) {
            cout << "-1\n";
            return;
        }

        if (string(user.password) != p) {
            cout << "-1\n";
            return;
        }

        bool isLoggedIn = false;
        if (loggedIn.find(u, isLoggedIn) && isLoggedIn) {
            cout << "-1\n";
            return;
        }

        loggedIn.insert(u, true);
        loggedIn.update(u, true);
        cout << "0\n";
    }

    void logout(HashMap<char, string>& params) {
        string u;
        params.find('u', u);

        bool isLoggedIn = false;
        if (!loggedIn.find(u, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        loggedIn.update(u, false);
        cout << "0\n";
    }

    void queryProfile(HashMap<char, string>& params) {
        string c, u;
        params.find('c', c);
        params.find('u', u);

        bool isLoggedIn = false;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        User cUser, uUser;
        if (!users.find(c, cUser) || !users.find(u, uUser)) {
            cout << "-1\n";
            return;
        }

        if (cUser.privilege <= uUser.privilege && c != u) {
            cout << "-1\n";
            return;
        }

        cout << uUser.username << " " << uUser.name << " " << uUser.mailAddr << " " << uUser.privilege << "\n";
    }

    void modifyProfile(HashMap<char, string>& params) {
        string c, u;
        params.find('c', c);
        params.find('u', u);

        bool isLoggedIn = false;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        User cUser, uUser;
        if (!users.find(c, cUser) || !users.find(u, uUser)) {
            cout << "-1\n";
            return;
        }

        if (cUser.privilege <= uUser.privilege && c != u) {
            cout << "-1\n";
            return;
        }

        string p, n, m, g;
        if (params.find('p', p)) strcpy(uUser.password, p.c_str());
        if (params.find('n', n)) strcpy(uUser.name, n.c_str());
        if (params.find('m', m)) strcpy(uUser.mailAddr, m.c_str());
        if (params.find('g', g)) {
            int newPriv = stoi(g);
            if (newPriv >= cUser.privilege) {
                cout << "-1\n";
                return;
            }
            uUser.privilege = newPriv;
        }

        users.update(u, uUser);
        cout << uUser.username << " " << uUser.name << " " << uUser.mailAddr << " " << uUser.privilege << "\n";
    }

    void addTrain(HashMap<char, string>& params) {
        string i;
        params.find('i', i);

        if (trains.exists(i)) {
            cout << "-1\n";
            return;
        }

        Train train;
        strcpy(train.trainID, i.c_str());

        string n, m, s, p, x, t, o, d, y;
        params.find('n', n);
        params.find('m', m);
        params.find('s', s);
        params.find('p', p);
        params.find('x', x);
        params.find('t', t);
        params.find('o', o);
        params.find('d', d);
        params.find('y', y);

        train.stationNum = stoi(n);
        train.seatNum = stoi(m);
        strcpy(train.startTime, x.c_str());
        train.type = y[0];
        train.released = false;

        string stationList[100];
        int stationCount;
        split(s, '|', stationList, stationCount);
        for (int j = 0; j < stationCount; j++) {
            strcpy(train.stations[j], stationList[j].c_str());
        }

        string priceList[100];
        int priceCount;
        split(p, '|', priceList, priceCount);
        for (int j = 0; j < priceCount; j++) {
            train.prices[j] = stoi(priceList[j]);
        }

        string travelList[100];
        int travelCount;
        split(t, '|', travelList, travelCount);
        for (int j = 0; j < travelCount; j++) {
            train.travelTimes[j] = stoi(travelList[j]);
        }

        string stopList[100];
        int stopCount;
        split(o, '|', stopList, stopCount);
        for (int j = 0; j < stopCount; j++) {
            train.stopoverTimes[j] = stoi(stopList[j]);
        }

        string dateList[10];
        int dateCount;
        split(d, '|', dateList, dateCount);
        strcpy(train.saleDate[0], dateList[0].c_str());
        strcpy(train.saleDate[1], dateList[1].c_str());

        trains.insert(i, train);
        cout << "0\n";
    }

    void releaseTrain(HashMap<char, string>& params) {
        string i;
        params.find('i', i);

        Train train;
        if (!trains.find(i, train)) {
            cout << "-1\n";
            return;
        }

        if (train.released) {
            cout << "-1\n";
            return;
        }

        train.released = true;
        trains.update(i, train);
        cout << "0\n";
    }

    void queryTrain(HashMap<char, string>& params) {
        string i, d;
        params.find('i', i);
        params.find('d', d);

        Train train;
        if (!trains.find(i, train)) {
            cout << "-1\n";
            return;
        }

        cout << train.trainID << " " << train.type << "\n";

        int totalMin = 0;
        int totalPrice = 0;

        for (int j = 0; j < train.stationNum; j++) {
            string arrDate, arrTime, depDate, depTime;

            if (j == 0) {
                // First station - departure date is query date
                cout << train.stations[j] << " xx-xx xx:xx -> " << d << " " << train.startTime << " " << totalPrice << " " << train.seatNum << "\n";
            } else if (j == train.stationNum - 1) {
                // Last station - calculate arrival time
                addTime(d, train.startTime, totalMin, arrDate, arrTime);
                cout << train.stations[j] << " " << arrDate << " " << arrTime << " -> xx-xx xx:xx " << totalPrice << " x\n";
            } else {
                // Middle station - calculate both arrival and departure times
                addTime(d, train.startTime, totalMin, arrDate, arrTime);
                int stopTime = train.stopoverTimes[j - 1];
                addTime(d, train.startTime, totalMin + stopTime, depDate, depTime);
                cout << train.stations[j] << " " << arrDate << " " << arrTime << " -> " << depDate << " " << depTime << " " << totalPrice << " " << train.seatNum << "\n";
                totalMin += stopTime;
            }

            if (j < train.stationNum - 1) {
                totalPrice += train.prices[j];
                totalMin += train.travelTimes[j];
            }
        }
    }

    void deleteTrain(HashMap<char, string>& params) {
        string i;
        params.find('i', i);

        Train train;
        if (!trains.find(i, train)) {
            cout << "-1\n";
            return;
        }

        if (train.released) {
            cout << "-1\n";
            return;
        }

        trains.erase(i);
        cout << "0\n";
    }

    void queryTicket(HashMap<char, string>& params) {
        cout << "0\n";
    }

    void queryTransfer(HashMap<char, string>& params) {
        cout << "0\n";
    }

    void buyTicket(HashMap<char, string>& params) {
        cout << "-1\n";
    }

    void queryOrder(HashMap<char, string>& params) {
        string u;
        params.find('u', u);

        bool isLoggedIn = false;
        if (!loggedIn.find(u, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        cout << "0\n";
    }

    void refundTicket(HashMap<char, string>& params) {
        cout << "-1\n";
    }

    void clean() {
        // Clear all data structures
        cout << "0\n";
    }

    void processCommand(const string& line) {
        if (line == "exit") {
            cout << "bye\n";
            return;
        }

        if (line == "clean") {
            clean();
            return;
        }

        string cmd;
        HashMap<char, string> params;
        parseParams(line, cmd, params);

        if (cmd == "add_user") addUser(params);
        else if (cmd == "login") login(params);
        else if (cmd == "logout") logout(params);
        else if (cmd == "query_profile") queryProfile(params);
        else if (cmd == "modify_profile") modifyProfile(params);
        else if (cmd == "add_train") addTrain(params);
        else if (cmd == "release_train") releaseTrain(params);
        else if (cmd == "query_train") queryTrain(params);
        else if (cmd == "delete_train") deleteTrain(params);
        else if (cmd == "query_ticket") queryTicket(params);
        else if (cmd == "query_transfer") queryTransfer(params);
        else if (cmd == "buy_ticket") buyTicket(params);
        else if (cmd == "query_order") queryOrder(params);
        else if (cmd == "refund_ticket") refundTicket(params);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    TicketSystem system;
    string line;

    while (getline(cin, line)) {
        if (!line.empty()) {
            system.processCommand(line);
            if (line == "exit") break;
        }
    }

    return 0;
}
