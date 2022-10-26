#include "word2vec.cpp"

int main(int argc, char const *argv[]){
    Word2Vec word2Vec("test/test.txt");
    word2Vec.debugMode = 0;
    word2Vec.numThreads = 2;
    word2Vec.iterTimes = 10;
    if (argc % 2 == 0) {
        cout << "Please input valid argument" << endl;
        return 0;
    } else {
        int index = 1;
        while (index < argc) {
            if (argv[index] == "-f") word2Vec.trainFile = argv[index + 1];
            else if (argv[index] == "-m") word2Vec.debugMode = stoi(argv[index + 1]);
            else if (argv[index] == "-t") word2Vec.numThreads = stoi(argv[index + 1]);
            else if (argv[index] == "-i") word2Vec.iterTimes = stoi(argv[index + 1]);
            else if (argv[index] == "-d")word2Vec.layer1Size = stoi(argv[index + 1]);
            else if (argv[index] == "-hs")word2Vec.layer1Size = stoi(argv[index + 1]);
            else if (argv[index] == "-ns")word2Vec.layer1Size = stoi(argv[index + 1]);
            index += 2;
        }
    }
    word2Vec.init();
    trainModel(word2Vec);
    word2Vec.saveResult();
    verifyResult(word2Vec);
    findSimiliarWord(word2Vec, "man");
    return 0;
}
