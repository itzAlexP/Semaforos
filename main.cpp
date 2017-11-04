#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <string.h>

#define MAX 100
#define SIZE_TABLERO 64
#define SIZE_FILA_TABLERO 8
#define LADO_CASILLA 64
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

#define SIZE_TABLERO 64
#define LADO_CASILLA 64
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

enum
TipoProceso{PADRE, ROJO, AZUL};
TipoProceso quienSoy;

char
tablero[SIZE_TABLERO];

float
posicionesTrenRojo[6][2]
{
    {2.f,2.f},
    {2.f,3.f},
    {2.f,4.f},
    {2.f,5.f},
    {2.f,6.f},
    {2.f,7.f}
},

posicionesTrenAzul[4][2]
{
    {4.f,2.f},
    {5.f,2.f},
    {6.f,2.f},
    {7.f,2.f}

};


/**
 * Si guardamos las posiciones de las piezas con valores del 0 al 7,
 * esta función las transforma a posición de ventana (pixel), que va del 0 al 512
 */

sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
    return sf::Vector2f(_position.x*LADO_CASILLA+OFFSET_AVATAR, _position.y*LADO_CASILLA+OFFSET_AVATAR);
}

//Pintamos por pantalla
void DibujarTrenes()
{
    sf::Vector2f casillaOrigen, casillaDestino;
    bool casillaMarcada=false;

    sf::RenderWindow window(sf::VideoMode(512,512), "Carretera y manta");
    while(window.isOpen())
    {
        //Captura de eventos
        sf::Event event;
        while (window.pollEvent(event)) {}

        window.clear();

        //Pintar tablero
        for (int i =0; i<8; i++)
        {
            for(int j = 0; j<8; j++)
            {
                sf::RectangleShape casilla(sf::Vector2f(LADO_CASILLA,LADO_CASILLA));
                casilla.setFillColor(sf::Color(65, 65, 65));

                if(i%2 == 0)
                {
                    //Empieza por el gris
                    if (j%2 == 0)
                    {
                        casilla.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
                        window.draw(casilla);
                    }
                }
                else
                {
                    //Empieza por el negro
                    if (j%2 == 1)
                    {
                        casilla.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
                        window.draw(casilla);
                    }
                }
            }
        }

        //Pintamos tren azul
        sf::CircleShape trenAzul(RADIO_AVATAR);
        trenAzul.setFillColor(sf::Color::Blue);

        for(int i = 0; i < 4; i++)
        {

            sf::Vector2f posicionTrenAzul(posicionesTrenAzul[i][0],posicionesTrenAzul[i][1]);
            posicionTrenAzul = BoardToWindows(posicionTrenAzul);
            trenAzul.setPosition(posicionTrenAzul);
            window.draw(trenAzul);

        }

        //Pintamos tren rojo
        sf::CircleShape trenRojo(RADIO_AVATAR);
        trenRojo.setFillColor(sf::Color::Red);

        for(int i = 0; i < 6; i++)
        {

            sf::Vector2f posicionTrenRojo(posicionesTrenRojo[i][0],posicionesTrenRojo[i][1]);
            posicionTrenRojo = BoardToWindows(posicionTrenRojo);
            trenRojo.setPosition(posicionTrenRojo);
            window.draw(trenRojo);

        }

        window.display();
    }

}

int main()
{
//TODO reservar memoria compartida

//TODO Inicializar semaforos

//TODO Crear tren rojo y tren azul


 //Pintar trenes
    DibujarTrenes();
    return 0;
}
