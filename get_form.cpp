#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

std::string decode_url(const std::string& str) 
{
    std::string decoded;
    size_t i = 0;
    while (i < str.size()) 
    {
        if (str[i] == '%') {
            if (i + 2 < str.size()) 
            {
                int value;
                std::istringstream iss(str.substr(i + 1, 2));
                if (iss >> std::hex >> value) 
                {
                    decoded += static_cast<char>(value);
                    i += 3;
                    continue;
                }
            }
        } 
        else if (str[i] == '+') 
        {
            decoded += ' ';
        } 
        else 
        {
            decoded += str[i];
        }
        ++i;
    }
    return decoded;
}

bool verifierLogin(const std::string& login, const std::string& mot_de_passe) 
{
    std::ifstream fichier("/var/www/get_formulaire/logins.txt");
    std::string stored_login, stored_mot_de_passe;

    if (!fichier.is_open()) 
    {
        std::cerr << "Erreur d'ouverture du fichier logins.txt" << std::endl;
        return false;
    }

    if (std::getline(fichier, stored_login) && std::getline(fichier, stored_mot_de_passe)) 
    {
        return (stored_login == login && stored_mot_de_passe == mot_de_passe);
    }

    return false;
}

int main() 
{
    std::cout << "Content-type: text/html\n\n";
    std::cout << "<html><head>";
    std::cout << "<link rel='stylesheet' type='text/css' href='../style.css'>";
    std::cout << "</head><body>";

    char* query = getenv("QUERY_STRING");

    if (query) 
    {
        std::string query_str(query);

        size_t login_pos = query_str.find("login=");
        size_t pass_pos = query_str.find("&mot_de_passe=");

        if (login_pos != std::string::npos && pass_pos != std::string::npos) 
        {
            std::string login = decode_url(query_str.substr(login_pos + 6, pass_pos - (login_pos + 6)));
            std::string mot_de_passe = decode_url(query_str.substr(pass_pos + 14));

            if (verifierLogin(login, mot_de_passe)) 
            {
                std::cout << "<h1>Bienvenue sur mon site, " << login << "!</h1>";
            } 
            else 
            {
                std::cout << "<h1>Informations incorrectes.</h1>";
                std::cout << "<form method='get' action='/cgi-bin/get_form.cgi'>";
                std::cout << "<label for='login'>Login : </label>";
                std::cout << "<input type='text' id='login' name='login' value='" << login << "' required><br><br>";
                std::cout << "<label for='mot_de_passe'>Mot de passe : </label>";
                std::cout << "<input type='password' id='mot_de_passe' name='mot_de_passe' value='" << mot_de_passe << "' required><br><br>";
                std::cout << "<div class=\"connexion\"><input type='submit' value='Se connecter'></div>";
                std::cout << "</form>";
            }
        } 
        else 
        {
            std::cout << "<h1>Erreur : Donnees de formulaire manquantes.</h1>";
        }
    } 
    else 
    {
        std::cout << "<h1>Erreur : Aucune donnee reçue.</h1>";
    }

    std::cout << "</body></html>";
    return 0;
}
