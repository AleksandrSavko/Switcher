#define PATH_EN2 "/home/aleksandr/Рабочий стол/Курсовая работа/src/en/proto"
#define PATH_EN3 "/home/aleksandr/Рабочий стол/Курсовая работа/src/en/proto3"
#define PATH_RU2 "/home/aleksandr/Рабочий стол/Курсовая работа/src/ru/proto"
#define PATH_RU3 "/home/aleksandr/Рабочий стол/Курсовая работа/src/ru/proto3"


#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h> 
#include <fstream>
#include <iostream> 
#include <vector>
#include <filesystem>
#include <cstring>
#include <err.h>
#include <string>
#include <unordered_map>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <unistd.h>
#include <cstdlib>
#include <map>
#include <codecvt>
#include <locale>

//функция проверки текущей раскладки клавиатуры
bool checking_current_layout()
{
    int result;
    char *language;
    auto display = XkbOpenDisplay( getenv( "DISPLAY" ), NULL, NULL, NULL, NULL, &result );
    auto keyboard = XkbAllocKeyboard();
    XkbGetNames( display, XkbGroupNamesMask, keyboard );
    XkbStateRec state;
    XkbGetState( display, XkbUseCoreKbd, &state );
    language=XGetAtomName(display, keyboard->names->groups[state.group]);
    if (strcmp(language,"Russian") ==0 ) return false;
    else return true;
}

// Функция для проверки русской строки по словарю 
 bool word_checking_RU_Proto2(std::string str, std::string str3)
 {
     std::ifstream inputFile(PATH_RU2);

    if (!inputFile.is_open()) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(inputFile, line)) {
       if (line.compare(str)==0) return false;
    }
    inputFile.close();

    std::ifstream inputFile3(PATH_RU3);

    if (!inputFile3.is_open()) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return 1;
    }

    std::string line3;

    while (std::getline(inputFile3, line3)) {
       if (line3.compare(str3)==0) return false;
    }
    inputFile3.close();

    return true;
 }

// Функция для проверки английской строки по словарю 
 bool word_checking_US_Proto2(std::string str, std::string str3)
 {
     std::ifstream inputFile(PATH_EN2);

    if (!inputFile.is_open()) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(inputFile, line)) {
       if (line.compare(str)==0) return false;
    }
    inputFile.close();

    std::ifstream inputFile3(PATH_EN3);

    if (!inputFile3.is_open()) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return 1;
    }

    std::string line3;

    while (std::getline(inputFile3, line3)) {
       if (line3.compare(str3)==0) return false;
    }
    inputFile3.close();

    return true;
 }

//функция смены языка
void language(Display *display) {
    
    
    KeyCode keycode_super = XKeysymToKeycode(display, XK_Super_L);  
    KeyCode keycode_space = XKeysymToKeycode(display, XK_space);
    XTestFakeKeyEvent(display, keycode_super, True, 0);
    XTestFakeKeyEvent(display, keycode_space, True, 0); 
    XTestFakeKeyEvent(display, keycode_super, False, 0);
    XTestFakeKeyEvent(display, keycode_space, False, 0);
    XFlush(display);

}

//Функция записи исправленной строки в буфер обмена
int copy_clipboard(std::string textTocopy, Display *display) 
{
    std::string command = "echo '" + textTocopy + "' | xclip -selection clipboard";
    std::system(command.c_str());
    return 0;
    
}

//функция вставки исправленной строки
void paste_word(Display *display) {
    
    
    KeyCode keycode_Control = XKeysymToKeycode(display, XK_Control_L);  
    KeyCode keycode_V = XKeysymToKeycode(display, XK_V);
    XTestFakeKeyEvent(display, keycode_Control, True, 0);
    XTestFakeKeyEvent(display, keycode_V, True, 0); 
    XTestFakeKeyEvent(display, keycode_Control, False, 0);
    XTestFakeKeyEvent(display, keycode_V, False, 0);
    XFlush(display);

}

//функция удаления неверного фрагмента
void delete_word(Display *display, size_t count) {
 
    for (size_t i = 0; i < count; i++)
    {
        KeyCode keycode = XKeysymToKeycode(display, XK_BackSpace); 
        XTestFakeKeyEvent(display, keycode, True, 0);
        XTestFakeKeyEvent(display, keycode, False, 0);
        XFlush(display);
    }
}
// Функция для замены английских букв на русские 
std::string replaceEnglishWithRussian(const std::string& input, const std::unordered_map<char, std::string>& keymap) {
    std::string result = input;

    for (size_t i = 0; i < result.length(); ++i) {
        // Проверяем, есть ли текущий символ в keymap
        char c = result[i];
        if (keymap.find(c) != keymap.end()) {
            // Если есть, заменяем его на соответствующую строку из keymap
            result.replace(i, 1, keymap.at(c));
        }
    }
    return result;
}

// Функция для замены русских букв на английские 
std::wstring replaceRussianWithEnglish(const std::wstring& input, const std::map<wchar_t, wchar_t>& charMap) {
    std::wstring result;

    for (wchar_t c : input) {
        auto it = charMap.find(c);
        if (it != charMap.end()) {
            result += it->second;
        } else {
            result += c;
        }
    }

    return result;
}

 std::string erase (int i,std::string str)
 {
    if (i>=3) str.erase(0,1);
    return str;
 }

int main()
{
    // Создаем keymap для замены английских букв на русские
    std::unordered_map <char, std::string> keymap = {
        {'q', "й"},
        {'w', "ц"},
        {'e', "у"},
        {'r', "к"},
        {'t', "е"},
        {'y', "н"},
        {'u', "г"},
        {'i', "ш"},
        {'o', "щ"},
        {'p', "з"},
        {'[', "х"},
        {']', "ъ"},
        {'a', "ф"},
        {'s', "ы"},
        {'d', "в"},
        {'f', "а"},
        {'g', "п"},
        {'h', "р"},
        {'j', "о"},
        {'k', "л"},
        {'l', "д"},
        {';', "ж"},
        {'\'', "ж"},
        {'z', "я"},
        {'x', "ч"},
        {'c', "с"},
        {'v', "м"},
        {'b', "и"},
        {'n', "т"},
        {'m', "ь"},
        {',', "б"},
        {'.', "ю"},
        
        
    };

        std::map<wchar_t, wchar_t> keyboardMapping;
    keyboardMapping[L'й'] = L'q';
    keyboardMapping[L'ц'] = L'w';
    keyboardMapping[L'у'] = L'e';
    keyboardMapping[L'к'] = L'r';
    keyboardMapping[L'е'] = L't';
    keyboardMapping[L'н'] = L'y';
    keyboardMapping[L'г'] = L'u';
    keyboardMapping[L'ш'] = L'i';
    keyboardMapping[L'щ'] = L'o';
    keyboardMapping[L'з'] = L'p';
    keyboardMapping[L'х'] = L'[';
    keyboardMapping[L'ъ'] = L']';
    keyboardMapping[L'ф'] = L'a';
    keyboardMapping[L'ы'] = L's';
    keyboardMapping[L'в'] = L'd';
    keyboardMapping[L'а'] = L'f';
    keyboardMapping[L'п'] = L'g';
    keyboardMapping[L'р'] = L'h';
    keyboardMapping[L'о'] = L'j';
    keyboardMapping[L'л'] = L'k';
    keyboardMapping[L'д'] = L'l';
    keyboardMapping[L'ж'] = L';';
    keyboardMapping[L'э'] = L'\'';
    keyboardMapping[L'я'] = L'z';
    keyboardMapping[L'ч'] = L'x';
    keyboardMapping[L'с'] = L'c';
    keyboardMapping[L'м'] = L'v';
    keyboardMapping[L'и'] = L'b';
    keyboardMapping[L'т'] = L'n';
    keyboardMapping[L'ь'] = L'm';
    keyboardMapping[L'б'] = L',';
    keyboardMapping[L'ю'] = L'.';

     std::map<KeySym, std::string> keysymToTextEnglish;
    keysymToTextEnglish[XK_a] = "a";
    keysymToTextEnglish[XK_b] = "b";
    keysymToTextEnglish[XK_c] = "c";
    keysymToTextEnglish[XK_d] = "d";
    keysymToTextEnglish[XK_e] = "e";
    keysymToTextEnglish[XK_f] = "f";
    keysymToTextEnglish[XK_g] = "g";
    keysymToTextEnglish[XK_h] = "h";
    keysymToTextEnglish[XK_i] = "i";
    keysymToTextEnglish[XK_j] = "j";
    keysymToTextEnglish[XK_k] = "k";
    keysymToTextEnglish[XK_l] = "l";
    keysymToTextEnglish[XK_m] = "m";
    keysymToTextEnglish[XK_n] = "n";
    keysymToTextEnglish[XK_o] = "o";
    keysymToTextEnglish[XK_p] = "p";
    keysymToTextEnglish[XK_q] = "q";
    keysymToTextEnglish[XK_r] = "r";
    keysymToTextEnglish[XK_s] = "s";
    keysymToTextEnglish[XK_t] = "t";
    keysymToTextEnglish[XK_u] = "u";
    keysymToTextEnglish[XK_v] = "v";
    keysymToTextEnglish[XK_w] = "w";
    keysymToTextEnglish[XK_x] = "x";
    keysymToTextEnglish[XK_y] = "y";
    keysymToTextEnglish[XK_z] = "z";
    keysymToTextEnglish[XK_space] = " ";
    keysymToTextEnglish[XK_comma] = ","; 
    keysymToTextEnglish[XK_period] = "."; 
    keysymToTextEnglish[XK_bracketleft] = "[";
    keysymToTextEnglish[XK_bracketright] = "]"; 
    keysymToTextEnglish[XK_semicolon] = ";"; 
    keysymToTextEnglish[XK_apostrophe] = "'"; 
    keysymToTextEnglish[XK_slash] = "/"; 



    std::map<KeySym, std::string> keysymToText;
    keysymToText[XK_Cyrillic_a] = "а";
    keysymToText[XK_Cyrillic_be] = "б";
    keysymToText[XK_Cyrillic_ve] = "в";
    keysymToText[XK_Cyrillic_ghe] = "г";
    keysymToText[XK_Cyrillic_de] = "д";
    keysymToText[XK_Cyrillic_ie] = "е";
    keysymToText[XK_Cyrillic_zhe] = "ж";
    keysymToText[XK_Cyrillic_ze] = "з";
    keysymToText[XK_Cyrillic_i] = "и";
    keysymToText[XK_Cyrillic_shorti] = "й";
    keysymToText[XK_Cyrillic_ka] = "к";
    keysymToText[XK_Cyrillic_el] = "л";
    keysymToText[XK_Cyrillic_em] = "м";
    keysymToText[XK_Cyrillic_en] = "н";
    keysymToText[XK_Cyrillic_o] = "о";
    keysymToText[XK_Cyrillic_pe] = "п";
    keysymToText[XK_Cyrillic_ya] = "я";
    keysymToText[XK_Cyrillic_er] = "р";
    keysymToText[XK_Cyrillic_es] = "с";
    keysymToText[XK_Cyrillic_te] = "т";
    keysymToText[XK_Cyrillic_u] = "у";
    keysymToText[XK_Cyrillic_ef] = "ф";
    keysymToText[XK_Cyrillic_ha] = "х";
    keysymToText[XK_Cyrillic_tse] = "ц";
    keysymToText[XK_Cyrillic_che] = "ч";
    keysymToText[XK_Cyrillic_sha] = "ш";
    keysymToText[XK_Cyrillic_shcha] = "щ";
    keysymToText[XK_Cyrillic_hardsign] = "ъ";
    keysymToText[XK_Cyrillic_yeru] = "ы";
    keysymToText[XK_Cyrillic_softsign] = "ь";
    keysymToText[XK_Cyrillic_e] = "э";
    keysymToText[XK_Cyrillic_yu] = "ю";
    keysymToText[XK_Cyrillic_ya] = "я";

   

    const char * hostname    = ":0";
 
    // Подключаемся к X-серверу
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp)
    {
        std::cerr << "Cannot open X display: " << hostname << std::endl;
        exit(1);
    }
 
    // Проверяем доступность Xinput
    int xiOpcode, queryEvent, queryError;
    if (! XQueryExtension(disp, "XInputExtension", &xiOpcode, &queryEvent, &queryError)) 
    {
        std::cerr << "X Input extension not available" << std::endl;
        exit(2);
    }
    
    // Регистрируем события
    Window root = DefaultRootWindow(disp);
    
    XIEventMask m;
    m.deviceid = XIAllMasterDevices;
    m.mask_len = XIMaskLen(XI_LASTEVENT);
    m.mask = (unsigned char*)calloc(m.mask_len, sizeof(char));
    XISetMask(m.mask, XI_RawKeyPress);
    
    XISelectEvents(disp, root, &m, 1);
    XSync(disp, false);
    free(m.mask);

    std::string buffer;
    std::string bufferRUS;
    std::string bufferString;
    std::string bufferStringRus;
    std::string resultRUS;
    std::string resultENG;
    int flag_v =0;
    int flag_space =0;

while (true) 
    {
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie*)&event.xcookie;
        XNextEvent(disp, &event);
        
        if (checking_current_layout())
        {
            
             if (XGetEventData(disp, cookie) &&
                cookie->type == GenericEvent &&
                cookie->extension == xiOpcode) 
            {

                if (cookie->evtype == XI_RawKeyPress) 
                {
                    XIRawEvent *ev = (XIRawEvent*)cookie->data;
                    
                    KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
                    if (NoSymbol == s) continue;

                    char *str = XKeysymToString(s);
                    if (NULL == str) continue;
                    if (flag_space && strcmp(str,"space") ==0)
                    {
                        flag_space=0;
                        continue;
                    }
                    if (flag_v && strcmp(str,"v") ==0)
                    {
                        flag_v=0;
                        continue;
                    }
                    if (strcmp(str,"space") ==0 ) buffer.clear();

                    auto it = keysymToTextEnglish.find(s);
                    
                    if (it != keysymToTextEnglish.end()) 
                    {
                        resultENG=it->second;
                    } 
                    buffer.append(resultENG); 
                    bufferString.append(resultENG); 
                    resultENG.clear();
                    
                    int counter=buffer.length();
                    
                    if (counter>=4) buffer.erase(0,(counter-3));
                    if (word_checking_US_Proto2(erase(counter,buffer), buffer)==false) 
                    {
                        
                        std::string result=replaceEnglishWithRussian(bufferString, keymap);
                        delete_word(disp, bufferString.length());
                        copy_clipboard(result,disp);
                        paste_word(disp);
                        language(disp);
                        flag_space=1;
                        flag_v=1;
                        
                        buffer.clear();
                        bufferString.clear();
                    }    
                }
            
            }
           
        }
        if  (!checking_current_layout())
        {
                
             if (XGetEventData(disp, cookie) &&
                cookie->type == GenericEvent &&
                cookie->extension == xiOpcode) 
            {

                if (cookie->evtype == XI_RawKeyPress) 
                {
                    XIRawEvent *ev = (XIRawEvent*)cookie->data;
                    
                    KeySym space = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
                    if (NoSymbol == space) continue;
                    char *str = XKeysymToString(space);
                    if (NULL == str) continue;
                
                    if (flag_space && strcmp(str,"space") ==0)
                    {
                        flag_space=0;
                        continue;
                    }
                    if (flag_v && strcmp(str,"v") ==0)
                    {
                        flag_v=0;
                        continue;
                    }
                                    
                    if (strcmp(str,"space") ==0 ) 
                    {
                        bufferStringRus.append(" ");
                        bufferRUS.clear();
                    }

                    KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 1, 0);
                    if (NoSymbol == s) continue;
                    auto it = keysymToText.find(s);
                    
                    if (it != keysymToText.end()) 
                    {
                        resultRUS=it->second;
                    }    
                                    
                    bufferRUS.append(resultRUS);
                    bufferStringRus.append(resultRUS);

                    int counter=bufferRUS.length();
                    if (counter>=6) bufferRUS.erase(0,(counter-6));
                    if (word_checking_RU_Proto2(erase(counter,bufferRUS), bufferRUS)==false) 
                    {
                        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                        std::wstring wide_str = converter.from_bytes(bufferStringRus);
                        std::wstring temorary_result=replaceRussianWithEnglish(wide_str, keyboardMapping);
                        std::string result = converter.to_bytes(temorary_result);

                        delete_word(disp, (bufferStringRus.length()/2));                    
                        copy_clipboard(result,disp);
                        paste_word(disp);
                        language(disp);
                        flag_space=1;
                        flag_v=1;
                        
                        bufferRUS.clear();
                        bufferStringRus.clear();
                        
                    }
                
                }
            
            }
        }
    }    
}
