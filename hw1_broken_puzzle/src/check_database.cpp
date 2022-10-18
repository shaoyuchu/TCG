#include "pattern_database.h"

int main() {
    string filePath = "pattern_database/1";
    PatternDatabase pd(filePath, Mode::read);
    unordered_map<bitset<BITSET_LEN>, int> result = pd.read();
    for (auto& iter : result) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                int pos = CONVERT_2D_2_1D(i, j);
                long num = -1;
                if (pos == 8 || pos == 10 || pos == 12) {
                    num = -1;
                } else {
                    bitset<100> mask(0b11111);
                    num = ((iter.first >> (pos * BIT_PER_CELL)) & mask).to_ulong();
                }
                cout << setw(4) << num << " ";
            }
            cout << endl;
        }
        // cout << iter.first.to_string() << " ";
        cout << iter.second << endl;
    }
}