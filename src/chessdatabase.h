#include "database.h"
#include <string>
#include <vector>
#ifndef CHESSDATABASE
#define CHESSDATABASE

class ChessDatabase : public Database {
public:
    ChessDatabase(string host, string user, string password, string database);
    vector<int> getEventIDs();
    vector<vector<int>> getGameIDsByEventID(int eventID);
    vector<vector<int>> getGameIDsByString(string str, int eventID = 0);
    vector<int> getPosIDsByGameID(int gameID);
    string getEventByID(int id);
    string getPlayerByID(int playerID);
    vector<string> getPlayersByGameID(int gameID);
private:
    vector<int> getPlayersByString(string str);
};

#endif // CHESSDATABASE

