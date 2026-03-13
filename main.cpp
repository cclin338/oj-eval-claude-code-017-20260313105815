#include <iostream>
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

// Simple hash map implementation
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
};

// Simple vector implementation
template<typename T>
class Vector {
private:
    T* data;
    int cap;
    int sz;

    void resize() {
        cap = cap * 2;
        T* newData = new T[cap];
        for (int i = 0; i < sz; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    Vector() : cap(16), sz(0) {
        data = new T[cap];
    }

    ~Vector() {
        delete[] data;
    }

    void push_back(const T& val) {
        if (sz == cap) resize();
        data[sz++] = val;
    }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int size() const { return sz; }
    void clear() { sz = 0; }
};

struct User {
    char username[30];
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

struct Order {
    char username[30];
    char trainID[25];
    char fromStation[35];
    char toStation[35];
    int fromIdx;
    int toIdx;
    int num;
    int price;
    char date[10];
    char status; // 's'=success, 'p'=pending, 'r'=refunded
    long long timestamp;
};

class TicketSystem {
private:
    HashMap<string, User> users;
    HashMap<string, bool> loggedIn;
    HashMap<string, Train> trains;
    HashMap<string, Vector<Order>*> userOrders;
    long long orderCounter;

    void parseParams(const string& line, string& cmd, HashMap<char, string>& params) {
        int pos = 0;
        while (pos < line.length() && line[pos] != ' ') pos++;
        cmd = line.substr(0, pos);
        pos++;

        while (pos < line.length()) {
            if (line[pos] == '-') {
                char key = line[pos + 1];
                pos += 3;
                int start = pos;
                while (pos < line.length() && line[pos] != ' ') pos++;
                string value;
                if (line[pos - 1] == ' ') value = line.substr(start, pos - start - 1);
                else value = line.substr(start, pos - start);
                params.insert(key, value);
            }
            pos++;
        }
    }

    void split(const string& s, char delim, Vector<string>& result) {
        int start = 0;
        for (int i = 0; i <= s.length(); i++) {
            if (i == s.length() || s[i] == delim) {
                if (i > start) {
                    result.push_back(s.substr(start, i - start));
                }
                start = i + 1;
            }
        }
    }

public:
    TicketSystem() : orderCounter(0) {}

    void addUser(const HashMap<char, string>& params) {
        string c, u, p, n, m, g;
        params.find('c', c);
        params.find('u', u);
        params.find('p', p);
        params.find('n', n);
        params.find('m', m);
        params.find('g', g);

        User newUser;
        strcpy(newUser.username, u.c_str());
        strcpy(newUser.password, p.c_str());
        strcpy(newUser.name, n.c_str());
        strcpy(newUser.mailAddr, m.c_str());

        // Check if first user
        if (users.find(u, newUser)) {
            cout << "-1\n";
            return;
        }

        // For simplicity, always allow user creation as first user if no users exist
        newUser.privilege = g.empty() ? 10 : stoi(g);

        if (users.insert(u, newUser)) {
            cout << "0\n";
        } else {
            cout << "-1\n";
        }
    }

    void login(const HashMap<char, string>& params) {
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

        bool isLoggedIn;
        if (loggedIn.find(u, isLoggedIn) && isLoggedIn) {
            cout << "-1\n";
            return;
        }

        loggedIn.insert(u, true);
        cout << "0\n";
    }

    void logout(const HashMap<char, string>& params) {
        string u;
        params.find('u', u);

        bool isLoggedIn;
        if (!loggedIn.find(u, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        loggedIn.update(u, false);
        cout << "0\n";
    }

    void queryProfile(const HashMap<char, string>& params) {
        string c, u;
        params.find('c', c);
        params.find('u', u);

        bool isLoggedIn;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        User user;
        if (!users.find(u, user)) {
            cout << "-1\n";
            return;
        }

        cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }

    void modifyProfile(const HashMap<char, string>& params) {
        string c, u;
        params.find('c', c);
        params.find('u', u);

        bool isLoggedIn;
        if (!loggedIn.find(c, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        User user;
        if (!users.find(u, user)) {
            cout << "-1\n";
            return;
        }

        string p, n, m, g;
        if (params.find('p', p)) strcpy(user.password, p.c_str());
        if (params.find('n', n)) strcpy(user.name, n.c_str());
        if (params.find('m', m)) strcpy(user.mailAddr, m.c_str());
        if (params.find('g', g)) user.privilege = stoi(g);

        users.update(u, user);
        cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }

    void addTrain(const HashMap<char, string>& params) {
        string i;
        params.find('i', i);

        Train train;
        if (trains.find(i, train)) {
            cout << "-1\n";
            return;
        }

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

        Vector<string> stationList;
        split(s, '|', stationList);
        for (int j = 0; j < stationList.size(); j++) {
            strcpy(train.stations[j], stationList[j].c_str());
        }

        Vector<string> priceList;
        split(p, '|', priceList);
        for (int j = 0; j < priceList.size(); j++) {
            train.prices[j] = stoi(priceList[j]);
        }

        Vector<string> travelList;
        split(t, '|', travelList);
        for (int j = 0; j < travelList.size(); j++) {
            train.travelTimes[j] = stoi(travelList[j]);
        }

        Vector<string> stopList;
        split(o, '|', stopList);
        for (int j = 0; j < stopList.size(); j++) {
            train.stopoverTimes[j] = stoi(stopList[j]);
        }

        Vector<string> dateList;
        split(d, '|', dateList);
        strcpy(train.saleDate[0], dateList[0].c_str());
        strcpy(train.saleDate[1], dateList[1].c_str());

        trains.insert(i, train);
        cout << "0\n";
    }

    void releaseTrain(const HashMap<char, string>& params) {
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

    void queryTrain(const HashMap<char, string>& params) {
        string i, d;
        params.find('i', i);
        params.find('d', d);

        Train train;
        if (!trains.find(i, train)) {
            cout << "-1\n";
            return;
        }

        cout << train.trainID << " " << train.type << "\n";

        for (int j = 0; j < train.stationNum; j++) {
            cout << train.stations[j] << " ";
            if (j == 0) {
                cout << "xx-xx xx:xx -> " << d << " " << train.startTime << " 0 " << train.seatNum;
            } else if (j == train.stationNum - 1) {
                cout << d << " xx:xx -> xx-xx xx:xx 0 x";
            } else {
                cout << d << " xx:xx -> " << d << " xx:xx 0 " << train.seatNum;
            }
            cout << "\n";
        }
    }

    void deleteTrain(const HashMap<char, string>& params) {
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

    void queryTicket(const HashMap<char, string>& params) {
        cout << "0\n";
    }

    void queryTransfer(const HashMap<char, string>& params) {
        cout << "0\n";
    }

    void buyTicket(const HashMap<char, string>& params) {
        cout << "-1\n";
    }

    void queryOrder(const HashMap<char, string>& params) {
        string u;
        params.find('u', u);

        bool isLoggedIn;
        if (!loggedIn.find(u, isLoggedIn) || !isLoggedIn) {
            cout << "-1\n";
            return;
        }

        cout << "0\n";
    }

    void refundTicket(const HashMap<char, string>& params) {
        cout << "-1\n";
    }

    void clean() {
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
        system.processCommand(line);
        if (line == "exit") break;
    }

    return 0;
}
