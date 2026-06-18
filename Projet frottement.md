# Projet montage pour mesure de coefficient de frottement.
<br>

<img
  src="./Assets/Montage%20mesure%20de%20frottement/Montage%20final.jpg"
  alt="Montage final"
  width="50%"
/>

<br>
<br>

## 1. Principe:

<br>

### On utilise un plan inclinable constitué de l'un des deux matériaux à tester. On place par-dessus le deuxième . En limite d'adhérence , la tangente de l'angle du plateau vaut le coefficient de frottement.

<img
  src="./Assets/Montage%20mesure%20de%20frottement/Schema%20principe.JPG"
  alt="Schema de principe"
  width="50%"
/>

<br>

## 2.En pratique :

<br>

### En pratique , j'ai utilisé un capteur à effet Hall avec une Arduino Uno reliée à un clavier et un écran pour pouvoir plus facilement réaliser des mesures précises .Le plateau s'incline grâce à une manivelle et un train d'engrenages.


![Capteur à effet Hall](./Assets/Montage%20mesure%20de%20frottement/Capteur%20à%20effet%20hall.JPG)


## 3.Conception:

<br>

### Les pièces violettes ont été réalisées par impression 3D pour avoir une meilleur liberté sur les géométries des pièces :

<img
  src="./Assets/Montage%20mesure%20de%20frottement/Impression%20des%20pieces.jpg"
  alt="Schema de principe"
  width="30%"
/>

<br>

### L'axe de rotation et les pignons sont isssus d'une ancienne imprimante :

<img
  src="./Assets/Montage%20mesure%20de%20frottement/Assemblage%20et%20train%20engrenage.jpg"
  alt="Schema de principe"
  width="30%"
/>

<br>

### Pour la partie électronique , j'ai réalisé un shield permettant de connecter un écran , un clavier et le capteur angulaire .Il y a aussi un connecteur pour un servo-moteur initialement prévu pour l'inclinaison du plateau.

<img
  src="./Assets/Montage%20mesure%20de%20frottement/Shield.jpg"
  alt="Schema de principe"
  width="50%"
/>

<br>

### Enfin le code Arduino :
<br>
<br>

``` cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "MT6701.h"

LiquidCrystal_I2C vLCD(0x3F, 16, 2);
MT6701 vCapteurAngle;
// Définition du clavier 4x4
const byte LIGNES = 4;
const byte COLONNES = 4;
char vTableTouche[LIGNES][COLONNES] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Broches des lignes et colonnes
byte brochesLignes[LIGNES] = {2,3,4,5};
byte brochesColonnes[COLONNES] = {6, 7, 8, 9};
Keypad vClavier = Keypad(makeKeymap(vTableTouche), brochesLignes, brochesColonnes, LIGNES, COLONNES);
float mylist[100];  // Tableau statique
int NbEnr = 0;      // Compteur d'éléments actifs
float vAngle_max;
bool vMesure = false;
int B_affichage = 0;
bool affichage = true;
//Capteur angulaire def:
// Définition de la broche analogique utilisée
const int capteurPin = A0;
int valeurinit = 0;
// Variables pour stocker la valeur lue et l'angle calculé
int valeurCapteur = 0;
float angle = 0.0;
void setup() {
  

  vLCD.init(); // Initialisation de l'afficheur
  vLCD.backlight(); // Allumage du rétroéclairage
  Serial.begin(115200);
  initialisation();
}

void loop() {
  char vTouche = vClavier.getKey();
   
  //Lancement mesure
  if (vTouche =='A'){
    init_Mesure();
    vAngle_max = 0;
    vMesure = true;
  }

  if (vTouche == 'B'){
    
  }

  //RAZ cycle de mesure
  if (vTouche == 'C'){
     initialisation();
  }
  
  //Sauvegarde mesure
  if (vTouche =='D'){
    ShowText("Result",10);
    vMesure = false;
    mylist[NbEnr]=vAngle_max;
    NbEnr+=1;
    ShowMoyenne_Nb();
  }

  if (vMesure){
      B_affichage += 1;
      if( B_affichage == 75){
        B_affichage = 0;
        if (affichage == true){
        ShowText("       ",10);
        affichage = false;
        }
        else{
          ShowText("Mesure",10);
          affichage = true;
        }
      }
     float vAngle = Mesure_angle();
      if (vAngle > vAngle_max){
        vAngle_max = vAngle ;
      }
      ShowAngle(vAngle_max);
  }
}

//Calcul de la moyenne
float moyenne(){
  if (NbEnr!=0){
  float Somme = 0;
  for (int i = 0; i < NbEnr; i++) {
  Somme += mylist[i];
  }
  return Somme / NbEnr;
  }else
  {return 0;}
}
void ShowAngle(float pVal) {
  char vBuffer[10]; // Buffer pour stocker la chaîne
  dtostrf(pVal, 5, 1, vBuffer); // Convertit le float en chaîne avec 2 décimales
  vLCD.setCursor(10, 1); 
  vLCD.print(vBuffer); // Affiche la chaîne
}

void ShowMoyenne_Nb() {
  vLCD.setCursor(3,1); 
  char vBuffer[10]; // Buffer pour stocker la chaîne
  dtostrf(moyenne(), 5, 1, vBuffer); // Convertit le float en chaîne avec 2 décimales
  vLCD.print(vBuffer);
  vLCD.setCursor(0,1);
  if (NbEnr < 10){
    vLCD.print("0");
    vLCD.print(NbEnr);
  }
  else{
  sprintf(vBuffer, "%2d", NbEnr);
  vLCD.print(vBuffer);
  }
  
}

void ShowText(String pLigne,int vpos) {
   vLCD.setCursor(vpos, 0); // Positionne le curseur à la 14ème colonne, 2ème ligne
  vLCD.print(pLigne); 
}
void ShowText1(String pLigne,int vpos) {
   vLCD.setCursor(vpos, 1); // Positionne le curseur à la 14ème colonne, 2ème ligne
  vLCD.print(pLigne); 
}

void initialisation(){
  vMesure = false ;
  valeurinit = analogRead(capteurPin);
  NbEnr=0;
  vLCD.clear();
  ShowText("Coefficient de",1);
  ShowText1("frottement",3);
}
void init_Mesure(){
  vLCD.clear();
  valeurinit = analogRead(capteurPin);
  ShowText("#  Moyen",0);
  ShowText("Mesure",10);
  ShowMoyenne_Nb();
}
float Mesure_angle(){
   // Lecture de la valeur analogique (0-1023)
  valeurCapteur = abs(analogRead(capteurPin) - valeurinit);

  // Conversion de la valeur en tension (0-5V)
  float tension = valeurCapteur * (5.0 / 1023.0);

  // Conversion de la tension en angle (0-360°)
  // Supposons que 0V = 0° et 5V = 360°
  angle = tension * (360.0 / 5.0);
  return angle ;

}
```

## Résultat

### Pour des plaques de titane brute , j'ai trouvé un coefficient de frottement moyen de 5.82 en comparaison dans la littérature scientifique ce coefficient est situé entre 0.4 et 0.6.