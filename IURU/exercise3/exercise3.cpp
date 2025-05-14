#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

class TransactionData {
private:
    map<int, vector<pair<string, int>>> TID; // 發票 ID、商品和數量
    map<string, vector<pair<int, vector<int>>>> combCount; // 組合名稱、發票ID、投影點
    map<string, int> itemPrices; // 商品名稱、單價
    map<string, vector<int>> setiu;
    map<string, vector<int>> setru;
public:
    // 初始化商品價格
    TransactionData() {
        itemPrices = {
            {"a", 1}, {"b", 2}, {"c", 1},
            {"d", 5}, {"e", 4}, {"f", 3}, {"g", 1}
        };
    }

    // 讀檔
    void LoadData(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            int TID_id;
            string line;
            string item;
            int count;
            while (getline(file, line)) {
                istringstream rd(line);
                if (rd >> TID_id >> item >> count) { // 依序讀取發票編號、商品名、數量
                    TID[TID_id].push_back({ item, count }); // 儲存商品和數量
                }
            }
            file.close();
        }
        else {
            cout << "開啟檔案錯誤" << endl;
        }
    }

   

    // 組合遞迴函數，計算總價格並儲存，並加入 TWU 砍樹策略
    void comb(int TID_id, const vector<pair<string, int>>& items, string set, vector<int> pos, int index, int minSupport, int iu) {
        if (!set.empty()) {
            
            // 計算當前組合之後的item的utility
            int ru = 0;
            for (int i = index; i < items.size(); ++i) {
                ru += itemPrices[items[i].first] * items[i].second;
            }
            if (iu + ru < minSupport) {
                return;
            }
            // 記錄當前組合和位置
            combCount[set].push_back({ TID_id, pos });

            
            
            
            // 保存當前的 iu 和計算出的 ru
            setiu[set].push_back(iu);
            setru[set].push_back(ru);
            
            
        }

        

        for (int i = index; i < items.size(); i++) {
            string newSet = set + "'" + items[i].first + "' ";
            vector<int> newPos = pos;
            newPos.push_back(i); // 紀錄位置
            int newPrice = iu + itemPrices[items[i].first] * items[i].second; // 計算當前組合的總價格

            // 遞迴下一層，注意這裡不再傳遞 ru，因為 ru 在遞迴內部計算
            comb(TID_id, items, newSet, newPos, i + 1, minSupport, newPrice);
        }
    }

    // 遍歷 TID 並計算組合
    void processTransactions(int minSupport) {
        for (const auto& entry : TID) {
            int TID_id = entry.first;
            const vector<pair<string, int>>& items = entry.second; // 讀取每張發票的商品和數量

            

            comb(TID_id, items, "", {}, 0, minSupport, 0); // 初始化價格為0
        }
    }

    // 顯示結果，包含總價格顯示
    void displayResults(int minSupport) const {
        int t = 0;
        for (const auto& entry : combCount) {
            //if (entry.second.size() >= minSupport) {
                cout << "組合:[" << entry.first << "]" << endl;
                t++;
                if (setiu.find(entry.first) == setiu.end() || setru.find(entry.first) == setru.end()) {
                    
                    continue;
                }
                for (const auto& pos : entry.second) {  // 讀 combCount 的 value
                    int TID_id = pos.first;
                    
                    

                    cout << "出現在發票: " << TID_id << ", 的投影點為:[" << pos.second.back() << "]" << endl;
                    
                }
                const auto& iu = setiu.at(entry.first);
                const auto& ru = setru.at(entry.first);
                for (size_t i = 0; i < iu.size(); ++i) {
                    cout << "iu: " << iu[i] << " ru: " << ru[i] << endl;
                }

            //}
        }
        cout << "共有: " << t << " 種符合條件組合" << endl; // 顯示符合條件的組合數量
    }
};

int main() {
    TransactionData data;
    string filename = "HUIdata.txt";
    data.LoadData(filename);

  

    int minSupport = 10;
    data.processTransactions(minSupport);
    data.displayResults(minSupport);

    return 0;
}
