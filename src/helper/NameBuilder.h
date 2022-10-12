#pragma once
#include <string>
#include <vector>

class NameBuilder
{
public:
    Real();

    std::string getAdjective(std::string name);
    std::string getRandomPlanetName(int syllables);
    std::string getRandomPlanetName();

private:
    typedef std::vector<std::string> slist;

    std::string rand(slist list);

    //Beginning syllables
    slist ccSyllables =
    {
        "Gany",
        "Fall", "Foll",
        "Zor", "Tad", "Tell", "Gadd", "Rotr",
    };

    //Unordered syllables (can appear at the beginning, end, or middle of a planet name)

    //Ending syllables starting with a consonant
    slist ecSyllables =
    {
        "ger",
        "piter", "turn", "res",
        "zor", "zub",
    };

    //Ending syllables starting with a vowel
    slist evSyllables =
    {
        "ars", "una", "ol", "uto",
        "otta", "aska", "ango", "eus", "ollo",
        "yor", "yat", "yog", "yit", "yir", "yoot", "yeet",
    };

};
