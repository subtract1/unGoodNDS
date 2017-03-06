#include "maker_code.h"

/* Maker codes are two byte strings that identify who made the damn game.
 * We don't care about this beyond it being one of many header fields that
 * determine who made what but it can be decoded so we might as well when
 * printing an analysis of ROMs.
 **/

const char * get_maker_code_name(char code[2])
{
    switch (code[0])
    {
        case '0':
            switch (code[1])
            {
                case '1': return "Nintendo";
                case '2': return "Rocket Games / Ajinomoto";
                case '3': return "Imagineer-Zoom";
                case '4': return "Gray Matter";
                case '5': return "Zamuse";
                case '6': return "Falcom";
                case '7': return "Enix";
                case '8': return "Capcom";
                case '9': return "Hot B Co.";
                case 'A': return "Jaleco";
                case 'B': return "Coconuts Japan";
                case 'C': return "Coconuts Japan / GX Media";
                case 'D': return "Micronet";
                case 'E': return "Technos";
                case 'F': return "Mebio Software";
                case 'G': return "Shouei System";
                case 'H': return "Starfish";
                case 'J': return "Mitsui Fudosan / Dentsu";
                case 'L': return "Warashi Inc.";
                case 'N': return "Nowpro";
                case 'P': return "Game Village";
                case 'Q': return "IE Institute";
            }
            break;
        case '1':
            switch (code[1])
            {
                case '2': return "Infocom";
                case '3': return "Electronic Arts Japan";
                case '5': return "Cobra Team";
                case '6': return "Human / Field";
                case '7': return "KOEI";
                case '8': return "Hudson Soft";
                case '9': return "S.C.P.";
                case 'A': return "Yanoman";
                case 'C': return "Tecmo Products";
                case 'D': return "Japan Glary Business";
                case 'E': return "Forum / OpenSystem";
                case 'F': return "Virgin Games (Japan)";
                case 'G': return "SMDE";
                case 'J': return "Daikokudenki";
                case 'P': return "Creatures Inc.";
                case 'Q': return "TDK Deep Impresion";
            }
            break;
        case '2':
            switch (code[1])
            {
                case '0': return "Zoo";
                case '1': return "Sunsoft / Tokai Engineering";
                case '2': return "Planning Office Wada / VR1 Japan";
                case '3': return "Micro World";
                case '5': return "San-X";
                case '6': return "Enix";
                case '7': return "Loriciel / Electro Brain";
                case '8': return "Kemco Japan";
                case '9': return "Seta";
                case 'A': return "Culture Brain";
                case 'C': return "Palsoft";
                case 'D': return "Visit Co.return Ltd.";
                case 'E': return "Intec";
                case 'F': return "System Sacom";
                case 'G': return "Poppo";
                case 'H': return "Ubisoft Japan";
                case 'J': return "Media Works";
                case 'K': return "NEC InterChannel";
                case 'L': return "Tam";
                case 'M': return "Jordan";
                case 'N': return "Smilesoft / Rocket";
                case 'Q': return "Mediakite";
            }
            break;
        case '3':
            switch (code[1])
            {
                case '0': return "Viacom";
                case '1': return "Carrozzeria";
                case '2': return "Dynamic";
                case '4': return "Magifact";
                case '5': return "Hect";
                case '6': return "Codemasters";
                case '7': return "Taito / GAGA Communications";
                case '8': return "Laguna";
                case '9': return "Telstar / Event / Taito ";
                case 'B': return "Arcade Zone Ltd.";
                case 'C': return "Entertainment International / Empire Software";
                case 'D': return "Loriciel";
                case 'E': return "Gremlin Graphics";
                case 'F': return "K. Amusement Leasing Co.";
            }
            break;
        case '4':
            switch (code[1])
            {
                case '0': return "Seika Corp.";
                case '1': return "Ubisoft Entertainment";
                case '2': return "Sunsoft US";
                case '4': return "Life Fitness";
                case '6': return "System 3";
                case '7': return "Spectrum Holobyte";
                case '9': return "IREM";
                case 'B': return "Raya Systems";
                case 'C': return "Renovation Products";
                case 'D': return "Malibu Games";
                case 'F': return "Eidos";
                case 'G': return "Playmates Interactive";
                case 'J': return "Fox Interactive";
                case 'K': return "Time Warner Interactive";
                case 'Q': return "Disney Interactive";
                case 'S': return "Black Pearl";
                case 'U': return "Advanced Productions";
                case 'X': return "GT Interactive";
                case 'Y': return "Rare";
                case 'Z': return "Crave Entertainment";
            }
            break;
        case '5':
            switch (code[1])
            {
                case '0': return "Absolute Entertainment";
                case '1': return "Acclaim";
                case '2': return "Activision";
                case '3': return "American Sammy";
                case '4': return "Take 2 Interactive / GameTek";
                case '5': return "Hi Tech";
                case '6': return "LJN Ltd.";
                case '8': return "Mattel";
                case 'A': return "Mindscape / Red Orb Entertainment";
                case 'B': return "Romstar";
                case 'C': return "Taxan";
                case 'D': return "Midway / Tradewest";
                case 'F': return "American Softworks";
                case 'G': return "Majesco Sales Inc.";
                case 'H': return "3DO";
                case 'K': return "Hasbro";
                case 'L': return "NewKidCo";
                case 'M': return "Telegames";
                case 'N': return "Metro3D";
                case 'P': return "Vatical Entertainment";
                case 'Q': return "LEGO Media";
                case 'S': return "Xicat Interactive";
                case 'T': return "Cryo Interactive";
                case 'W': return "Red Storm Entertainment";
                case 'X': return "Microids";
                case 'Z': return "Data Design / Conspiracy / Swing";
            }
            break;
        case '6':
            switch (code[1])
            {
                case '0': return "Titus";
                case '1': return "Virgin Interactive";
                case '2': return "Maxis";
                case '4': return "LucasArts Entertainment";
                case '7': return "Ocean";
                case '8': return "Bethesda Softworks";
                case '9': return "Electronic Arts";
                case 'B': return "Laser Beam";
                case 'E': return "Elite Systems";
                case 'F': return "Electro Brain";
                case 'G': return "The Learning Company";
                case 'H': return "BBC";
                case 'J': return "Software 2000";
                case 'K': return "UFO Interactive Games";
                case 'L': return "BAM! Entertainment";
                case 'M': return "Studio 3";
                case 'Q': return "Classified Games";
                case 'S': return "TDK Mediactive";
                case 'U': return "DreamCatcher";
                case 'V': return "JoWood Produtions";
                case 'W': return "Sega";
                case 'X': return "Wannado Edition";
                case 'Y': return "LSP (Light & Shadow Prod.)";
                case 'Z': return "ITE Media";
            }
            break;
        case '7':
            switch (code[1])
            {
                case '0': return "Atari (Infogrames)";
                case '1': return "Interplay";
                case '2': return "JVC (US)";
                case '3': return "Parker Brothers";
                case '5': return "Sales Curve (Storm / SCI)";
                case '8': return "THQ";
                case '9': return "Accolade";
                case 'A': return "Triffix Entertainment";
                case 'C': return "Microprose Software";
                case 'D': return "Sierra / Universal Interactive";
                case 'F': return "Kemco";
                case 'G': return "Rage Software";
                case 'H': return "Encore";
                case 'J': return "Zoo";
                case 'K': return "Kiddinx";
                case 'L': return "Simon & Schuster Interactive";
                case 'M': return "Asmik Ace Entertainment Inc.";
                case 'N': return "Empire Interactive";
                case 'Q': return "Jester Interactive";
                case 'S': return "Rockstar Games";
                case 'T': return "Scholastic";
                case 'U': return "Ignition Entertainment";
                case 'V': return "Summitsoft";
                case 'W': return "Stadlbauer";
            }
            break;
        case '8':
            switch (code[1])
            {
                case '0': return "Misawa";
                case '1': return "Teichiku";
                case '2': return "Namco Ltd.";
                case '3': return "LOZC";
                case '4': return "KOEI";
                case '6': return "Tokuma Shoten Intermedia";
                case '7': return "Tsukuda Original";
                case '8': return "DATAM-Polystar";
                case 'B': return "Bullet-Proof Software";
                case 'C': return "Vic Tokai Inc.";
                case 'E': return "Character Soft";
                case 'F': return "I'Max";
                case 'G': return "Saurus";
                case 'J': return "General Entertainment";
                case 'N': return "Success";
                case 'P': return "Sega Japan";
            }
            break;
        case '9':
            switch (code[1])
            {
                case '0': return "Takara Amusement";
                case '1': return "Chunsoft";
                case '2': return "Video System / Mc O' River";
                case '3': return "BEC";
                case '5': return "Varie";
                case '6': return "Yonezawa / S'pal";
                case '7': return "Kaneko";
                case '9': return "Marvelous Entertainment";
                case 'A': return "Nichibutsu / Nihon Bussan";
                case 'B': return "Tecmo";
                case 'C': return "Imagineer";
                case 'F': return "Nova";
                case 'G': return "Take2 / Den'Z / Global Star";
                case 'H': return "Bottom Up";
                case 'J': return "Technical Group Laboratory";
                case 'L': return "Hasbro Japan";
                case 'N': return "Marvelous Entertainment";
                case 'P': return "Keynet Inc.";
                case 'Q': return "Hands-On Entertainment";
            }
            break;
        case 'A':
            switch (code[1])
            {
                case '0': return "Telenet";
                case '1': return "Hori";
                case '4': return "Konami";
                case '5': return "K. Amusement Leasing Co.";
                case '6': return "Kawada";
                case '7': return "Takara";
                case '9': return "Technos Japan Corp.";
                case 'A': return "JVC / Victor";
                case 'C': return "Toei Animation";
                case 'D': return "Toho";
                case 'F': return "Namco";
                case 'G': return "Media Rings Corporation";
                case 'H': return "J-Wing";
                case 'J': return "Pioneer LDC";
                case 'K': return "KID";
                case 'L': return "Mediafactory";
                case 'P': return "Infogrames / Hudson";
                case 'Q': return "Kiratto Ludic Inc.";
            }
            break;
        case 'B':
            switch (code[1])
            {
                case '0': return "Acclaim Japan";
                case '1': return "ASCII";
                case '2': return "Bandai";
                case '4': return "Enix";
                case '6': return "HAL Laboratory";
                case '7': return "SNK";
                case '9': return "Pony Canyon";
                case 'A': return "Culture Brain";
                case 'B': return "Sunsoft";
                case 'C': return "Toshiba EMI";
                case 'D': return "Sony Imagesoft";
                case 'F': return "Sammy";
                case 'G': return "Magical";
                case 'H': return "Visco";
                case 'J': return "Compile";
                case 'L': return "MTO Inc.";
                case 'N': return "Sunrise Interactive";
                case 'P': return "Global A Entertainment";
                case 'Q': return "Fuuki";
            }
            break;
        case 'C':
            switch (code[1])
            {
                case '0': return "Taito";
                case '2': return "Kemco";
                case '3': return "Square";
                case '4': return "Tokuma Shoten";
                case '5': return "Data East";
                case '6': return "Tonkin House / Tokyo Shoseki";
                case '8': return "Koei";
                case 'A': return "Konami / Ultra / Palcom";
                case 'B': return "NTVIC / VAP";
                case 'C': return "Use Co.return Ltd.";
                case 'D': return "Meldac";
                case 'E': return "Pony Canyon / FCI ";
                case 'F': return "Angel / Sotsu Agency / Sunrise";
                case 'G': return "Yumedia / Aroma Co.return Ltd";
                case 'J': return "Boss";
                case 'K': return "Axela / Crea-Tech";
                case 'L': return "Sekaibunka-Sha / Sumire Kobo / Marigul Management Inc.";
                case 'M': return "Konami Computer Entertainment Osaka";
                case 'N': return "NEC Interchannel";
                case 'P': return "Enterbrain";
                case 'Q': return "From Software";
            }
            break;
        case 'D':
            switch (code[1])
            {
                case '0': return "Taito / Disco";
                case '1': return "Sofel";
                case '2': return "Quest / Bothtec";
                case '3': return "Sigma";
                case '4': return "Ask Kodansha";
                case '6': return "Naxat";
                case '7': return "Copya System";
                case '8': return "Capcom Co.return Ltd.";
                case '9': return "Banpresto";
                case 'A': return "Tomy";
                case 'B': return "LJN Japan";
                case 'D': return "NCS";
                case 'E': return "Human Entertainment";
                case 'F': return "Altron";
                case 'G': return "Jaleco";
                case 'H': return "Gaps Inc.";
                case 'N': return "Elf";
                case 'Q': return "Compile Heart";
            }
            break;
        case 'E':
            switch (code[1])
            {
                case '0': return "Jaleco";
                case '2': return "Yutaka";
                case '3': return "Varie";
                case '4': return "T&E Soft";
                case '5': return "Epoch";
                case '7': return "Athena";
                case '8': return "Asmik";
                case '9': return "Natsume";
                case 'A': return "King Records";
                case 'B': return "Atlus";
                case 'C': return "Epic / Sony Records";
                case 'E': return "Information Global Service";
                case 'G': return "Chatnoir";
                case 'H': return "Right Stuff";
                case 'L': return "Spike";
                case 'M': return "Konami Computer Entertainment Tokyo";
                case 'N': return "Alphadream Corporation";
                case 'P': return "Sting";
                case 'S': return "Star-Fish";
            }
            break;
        case 'F':
            switch (code[1])
            {
                case '0': return "A Wave";
                case '1': return "Motown Software";
                case '2': return "Left Field Entertainment";
                case '3': return "Extreme Ent. Grp.";
                case '4': return "TecMagik";
                case '9': return "Cybersoft";
                case 'B': return "Psygnosis";
                case 'E': return "Davidson / Western Tech.";
                case 'K': return "The Game Factory";
                case 'L': return "Hip Games";
                case 'M': return "Aspyr";
                case 'P': return "Mastiff";
                case 'Q': return "iQue";
                case 'R': return "Digital Tainment Pool";
                case 'S': return "XS Games / Jack Of All Games";
                case 'T': return "Daiwon";
            }
            break;
        case 'G':
            switch (code[1])
            {
                case '0': return "Alpha Unit";
                case '1': return "PCCW Japan";
                case '2': return "Yuke's Media Creations";
                case '4': return "KiKi Co.return Ltd.";
                case '5': return "Open Sesame Inc.";
                case '6': return "Sims";
                case '7': return "Broccoli";
                case '8': return "Avex";
                case '9': return "D3 Publisher";
                case 'B': return "Konami Computer Entertainment Japan";
                case 'D': return "Square-Enix";
                case 'E': return "KSG";
                case 'F': return "Micott & Basara Inc.";
                case 'H': return "Orbital Media";
                case 'J': return "Detn8 Games";
                case 'L': return "Gameloft / Ubisoft";
                case 'M': return "Gamecock Media Group";
                case 'N': return "Oxygen Games";
                case 'T': return "505 Games";
                case 'Y': return "The Game Factory";
            }
            break;
        case 'H':
            switch (code[1])
            {
                case '1': return "Treasure";
                case '2': return "Aruze";
                case '3': return "Ertain";
                case '4': return "SNK Playmore";
                case 'F': return "Level-5";
                case 'J': return "Genius Products";
                case 'Y': return "Reef Entertainment";
                case 'Z': return "Nordcurrent";
            }
            break;
        case 'I':
            switch (code[1])
            {
                case 'H': return "Yojigen";
            }
            break;
        case 'J':
            switch (code[1])
            {
                case '9': return "AQ Interactive";
                case 'F': return "Arc System Works";
                case 'J': return "Deep Silver";
                case 'W': return "Atari";
            }
            break;
        case 'K':
            switch (code[1])
            {
                case '6': return "Nihon System";
                case 'B': return "NIS America";
                case 'M': return "Deep Silver";
                case 'P': return "Purple Hills";
            }
            break;
        case 'L':
            switch (code[1])
            {
                case 'H': return "Trend Verlag / East Entertainment";
                case 'T': return "Legacy Interactive";
            }
            break;
        case 'M':
            switch (code[1])
            {
                case 'J': return "Mumbo Jumbo";
                case 'R': return "Mindscape";
                case 'S': return "Milestone / UFO Interactive";
                case 'T': return "Blast!";
            }
            break;
        case 'N':
            switch (code[1])
            {
                case '9': return "Terabox";
                case 'G': return "Nordic Games";
                case 'K': return "Neko Entertainment / Diffusion / Naps team";
                case 'P': return "Nobilis";
                case 'Q': return "Namco Bandai";
                case 'R': return "Data Design / Destineer Studios";
                case 'S': return "NIS America";
            }
            break;
        case 'O':
            break;
        case 'P':
            switch (code[1])
            {
                case 'L': return "Playlogic";
            }
            break;
        case 'Q':
            switch (code[1])
            {
                case 'C': return "Kadokawa Shoten";
            }
            break;
        case 'R':
            switch (code[1])
            {
                case 'M': return "Rondomedia";
                case 'S': return "Warner Bros. Interactive Entertainment Inc.";
                case 'T': return "RTL Games";
                case 'W': return "RealNetworks";
            }
            break;
        case 'S':
            switch (code[1])
            {
                case '5': return "Southpeak Interactive";
                case 'P': return "Blade Interactive Studios";
                case 'V': return "SevenGames";
                case 'Z': return "Storm City";
            }
            break;
        case 'T':
            switch (code[1])
            {
                case 'K': return "Tasuke / Works";
                case 'V': return "Tivola";
            }
            break;
        case 'U':
            switch (code[1])
            {
                case 'G': return "Metro 3D / Data Design";
            }
            break;
        case 'V':
            switch (code[1])
            {
                case 'N': return "Valcon Games";
                case 'P': return "Virgin Play";
                case 'Z': return "Little Orbit";
            }
            break;
        case 'W':
            switch (code[1])
            {
                case 'R': return "Warner Bros. Interactive Entertainment Inc.";
            }
            break;
        case 'X':
            switch (code[1])
            {
                case 'J': return "Xseed Games";
                case 'S': return "Aksys Games";
            }
            break;
        case 'Y':
            switch (code[1])
            {
                case 'T': return "Valcon Games";
            }
            break;
        case 'Z':
            switch (code[1])
            {
                case '4': return "Ntreev Soft";
                case 'A': return "WBA Interactive";
                case 'H': return "Internal Engine";
                case 'S': return "Zinkia";
                case 'W': return "Judo Baby";
                case 'X': return "Topware Interactive";
            }
            break;
    }

    return "??????";
}
