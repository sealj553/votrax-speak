#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>

#include <cstdio>

#include <pulse/simple.h>
#include <pulse/error.h>

#include <espeak/speak_lib.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;
//using std::iterator;
using std::ifstream;
using std::istreambuf_iterator;

const int BUF_SIZE = 128;

const string audio_dir = "../samples/";
const string audio_format = ".wav";

/*phonememode: bits0-3:
  0= just phonemes.
  1= include ties (U+361) for phoneme names of more than one letter.
  2= include zero-width-joiner for phoneme names of more than one letter.
  3= separate phonemes with underscore characters.
  bits 4-7:
  0= eSpeak's ascii phoneme names.
  1= International Phonetic Alphabet (as UTF-8 characters).*/
const int phoneme_mode = 3 | (1 << 4);

const vector<string> phoneme_names = {
    "EH3",
    "EH2",
    "EH1",
    "A2",
    "A1",
    "ZH",
    "AH2",
    "I3",
    "I2",
    "I1",
    "M",
    "N",
    "B",
    "V",
    "CH",
    "SH",
    "Z",
    "AW1",
    "NG",
    "AH1",
    "OO1",
    "OO",
    "L",
    "K",
    "J",
    "H",
    "G",
    "F",
    "D",
    "S",
    "A",
    "AY",
    "Y1",
    "UH3",
    "AH",
    "P",
    "O",
    "I",
    "U",
    "Y",
    "T",
    "R",
    "E",
    "W",
    "AE",
    "AE1",
    "AW2",
    "UH2",
    "UH1",
    "UH",
    "O2",
    "O1",
    "IU",
    "U1",
    "THV",
    "TH",
    "ER",
    "EH",
    "E1",
    "AW"
};

pa_simple *s = NULL;

vector<vector<char>> buffers;

//phoneme name to buffer
unordered_map<string, vector<char>*> name_to_buffer;

//ipa to votrax phoneme
unordered_map<string, string> ipa_to_votrax;

inline void sleep(int millis){
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

void init(){
    //ipa_to_votrax["I"] = "EH3";
    ipa_to_votrax["ˈɛ"] = "EH2";
    //ipa_to_votrax["ˈɛ"] = "EH1";
    ipa_to_votrax["ˈeɪ"] = "A2";
    //ipa_to_votrax["ˈeɪ"] = "A1";
    ipa_to_votrax["ʒ"]  = "ZH";
    ipa_to_votrax["ˈɒ"] = "AH2";
    //ipa_to_votrax["I"] = "I3";
    ipa_to_votrax["ˈɪ"]  = "I2";
    ipa_to_votrax["I"]   = "I1";
    ipa_to_votrax["m"]   = "M";
    ipa_to_votrax["n"]   = "N";
    ipa_to_votrax["b"]   = "B";
    ipa_to_votrax["v"]   = "V";
    ipa_to_votrax["tʃ"]  = "CH";
    ipa_to_votrax["ʃ"]   = "SH";
    ipa_to_votrax["z"]   = "Z";
    ipa_to_votrax["ˈɔː"] = "AW1";
    ipa_to_votrax["ŋ"]   = "NG";
    ipa_to_votrax["ˈɑː"] = "AH1";
    ipa_to_votrax["ˈʊ"]  = "OO1";
    ipa_to_votrax["ˈʊ"]  = "OO";
    ipa_to_votrax["l"]   = "L";
    ipa_to_votrax["k"]   = "K";
    ipa_to_votrax["dʒ"]  = "J";
    ipa_to_votrax["h"]   = "H";
    ipa_to_votrax["ɡ"]   = "G";
    ipa_to_votrax["f"]   = "F";
    ipa_to_votrax["d"]   = "D";
    ipa_to_votrax["s"]   = "S";
    //ipa_to_votrax["ˈeɪ"] = "A";
    ipa_to_votrax["ˈeɪ"] = "AY";
    ipa_to_votrax["j"]   = "Y1";
    ipa_to_votrax["ə"]   = "UH3";
    ipa_to_votrax["a"]   = "AH";
    ipa_to_votrax["p"]   = "P";
    ipa_to_votrax["ˈəʊ"] = "O";
    ipa_to_votrax["ˈɪ"]  = "I";
    ipa_to_votrax["ˈuː"] = "U";
    ipa_to_votrax["i"]   = "Y";
    ipa_to_votrax["t"]   = "T";
    ipa_to_votrax["ɹ"]   = "R";
    ipa_to_votrax["ˈiː"] = "E";
    ipa_to_votrax["w"]   = "W";
    //ipa_to_votrax["ˈa"]  = "AE";
    ipa_to_votrax["ˈa"]  = "AE1";
    ipa_to_votrax["ˈɒ"]  = "AW2";
    ipa_to_votrax["ɐ"]   = "UH2";
    //ipa_to_votrax["ˈʌ"]  = "UH1";
    ipa_to_votrax["ˈʌ"]  = "UH";
    ipa_to_votrax["ɔː"]  = "O2";
    ipa_to_votrax["ˈɔː"] = "O1";
    //ipa_to_votrax["uː"] = "IU";
    ipa_to_votrax["uː"]  = "U1";
    ipa_to_votrax["ð"]   = "THV";
    ipa_to_votrax["θ"]   = "TH";
    ipa_to_votrax["ˈɜː"] = "ER";
    ipa_to_votrax["ˈɛ"]  = "EH";
    ipa_to_votrax["ˈiː"] = "E1";
    ipa_to_votrax["ˈɔː"] = "AW";

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 1
    };

    int error;
    if(!(s = pa_simple_new(NULL, "phoneme", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        cout << "pa_simple_new() failed: " << pa_strerror(error) << endl;
    }

    buffers.reserve(phoneme_names.size());

    for(unsigned i = 0; i < phoneme_names.size(); ++i){
        string filename = audio_dir + phoneme_names[i] + audio_format;
        ////cout << "opening: " << filename << endl;

        ifstream input(filename, std::ios::binary);
        if(!input.is_open()){ cout << "error opening file" << endl; }

        buffers.emplace(buffers.begin() + i, vector<char>(istreambuf_iterator<char>(input), {}));;
        //cout << "size: " << buffers.back().size() << endl;
        name_to_buffer[phoneme_names[i]] = &(buffers[i]);
    }

    espeak_Initialize(espeak_AUDIO_OUTPUT(), 0, NULL, 0);
    espeak_SetVoiceByName("en");
}

void play_sound(vector<char> buffer){
    int error;
    //WAV header is 44 bytes
    //removing 256 at the end because there's extra data or something?
    if(pa_simple_write(s, &(buffer[44]), buffer.size() - 256, &error) < 0){
        cout << "pa_simple_write() failed: " << pa_strerror(error) << endl;
    }
    if(pa_simple_drain(s, &error) < 0){
        cout << "pa_simple_drain() failed: " << pa_strerror(error) << endl;
    }
}


void play(string &phoneme){
    string votrax = ipa_to_votrax[phoneme];
    cout << "playing " << phoneme << " aka " << votrax << endl;
    if(votrax != string("")){
        play_sound(*(name_to_buffer[votrax]));
        //sleep(100);
    }
}

string getPhonemes(string &input){
    const char *output = espeak_TextToPhonemes((const void**)&(input), espeakCHARS_UTF8, phoneme_mode);
    string out(output);
    free((void*)output);
    return out;
}

void parse(string &&str){
    str.erase(str.begin()); //remove first space
    cout << "phonemes: " << str << endl;
    vector<string> phonemes;

    const string delimiter = "_";
    size_t pos;
    while((pos = str.find(delimiter)) != string::npos || 
                (pos = str.find(" ")) != string::npos){
        string token = str.substr(0, pos);
        phonemes.push_back(token);
        str.erase(0, pos + 1);
    }
    phonemes.push_back(str);

    for(string &s : phonemes){
        play(s);
    }
}

int main(int argc, char **argv){
    if(argc < 2){
        cout << "Usage: ./ipa2chip \"phonemes\"" << endl;
        return 1;
    }
    init();
    //string input;
    //while(std::getline(cin, input)){
    //    parse(getPhonemes(input));
    //}
    for(int i = 1; i < argc; ++i){
        string input = argv[i];
        parse(getPhonemes(input));
        sleep(50);
    }
    pa_simple_free(s);
}
