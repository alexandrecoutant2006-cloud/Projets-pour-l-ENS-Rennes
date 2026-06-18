# Banc d'essai torsion et limiteur de couple
<br>

<img
  src="./Assets/Module%20limiteur%20de%20couple/Utilisation%20du%20système.jpg"
  alt="Illustration du projet"
  width="30%"
/>

## Composition:

<br>

### Ce projet se compose de trois modules différents :
     -Un support permettant d'appliquer un couple sur une barre à partir de masses
     -Un module de torsion (une barre dont on peut bloquer la rotation à une extremité équipée de jauges de déformation)
     -Un module limiteur de couple simulant un limiteur avec deux plaques en titane.

<br>

## Le support 
<br>

### Il se compose d'une poulie pour pouvoir grâce à des masses appliquer un moment pur ( la poulie permet d'avoir une force verticale ascendante ,et autre masse permet d'avoir une force descendante).Et de deux planches avec des goulottes pour pouvoir installer les modules.

<img
  src="./Assets/Banc%20essai.jpg"
  alt="Banc d'essai"
  width="30%"
/>

<br>


# Le module de torsion

<br>

## Partie mécanique:

### La structure du module est une barre en H permettant d'appliquer avec un bras de levier un couple à l'une des extrémitées et de bloquer la rotation de la barre de l'autre .

<img
 src="./Assets/Module%20torsion/Structure%20du%20module%20torsion.jpg"
  alt="Structure du module"
  width="50%" 
/>

<br>

### Bloquage en rotation

<img
 src="./Assets/Module%20torsion/Module%20torsion%20bloquage%20rotation.jpg"
  alt="Bloquage rotation"
  width="50%" 
/>

### Test de la partie mécanique :

<img
 src="./Assets/Module%20torsion/Module%20torsion%20sans%20la%20partie%20elec.jpg"
  alt="Essai mécanique"
  width="50%" 
/>
<br>

## Partie électronique:

<br>

### Pour avoir une mesure de la torsion j'ai utilisé deux jauges de déformations assemblées dans un pont de Wheatstone relié à un amplificateur HX711 puis à une carte Arduino.

<br>

jauge

<img
src="./Assets/Module%20torsion/jauge%20de%20déformation.jpg"
  alt="Structure du module"
  width="30%"
/>

Reste du circuit

<img
src="./Assets/Module%20torsion/Pont%20de%20Wheatstone.jpg"
  alt="Circuit"
  width="30%"
/>

<br>

## Code Arduino



```cpp
// ============================================================
// HX711 + Arduino Uno
// Lecture sur CANAL A, GAIN = 128 (25 pulses)
//
// Affiche :
// 1) E_raw        : code numérique brut (24 bits signé)
// 2) e_mV         : tension différentielle d'entrée e = INA+ - INA- (mV)
// 3) e_over_Vref  : ratio e / AVDD (sans unité)
//
// Connexions :
//   HX711 DT  -> A1
//   HX711 SCK -> A0
// ============================================================

const int DOUT_PIN = A1;   // HX711 Data
const int SCK_PIN  = A0;   // HX711 Clock

// Paramètres HX711
const float GAIN      = 128.0f;  // Canal A, gain 128
const float AVDD_VOLTS = 3.72f;  // Tension analogique HX711 (mesurée idéalement au multimètre)

// Constantes ADC HX711
// Sortie 24 bits signé -> pleine échelle numérique = ±2^23
const float ADC_FS = 8388608.0f; // 2^23

// Pleine échelle tension (canal A) : Vfs = ± (0.5 * AVDD) / GAIN
float VFS_VOLTS;

void setup() {
  Serial.begin(57600);
  pinMode(DOUT_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  digitalWrite(SCK_PIN, LOW);

  VFS_VOLTS = (0.5f * AVDD_VOLTS) / GAIN;

  Serial.println("E_raw\te_mV\te_over_Vref");
}

void loop() {

  // --- 1) Lecture du code numérique brut (signé) ---
  int32_t E_raw = readHX711_A128();

  // --- 2) Conversion en tension e (volts) ---
  // e = (E_raw / 2^23) * VFS
  float e_volts = ((float)E_raw / ADC_FS) * VFS_VOLTS;

  // --- 3) En mV ---
  float e_mV = 1000.0f * e_volts;

  // --- 4) Ratio e / AVDD (sans unité) ---
  float e_over_Vref = e_volts / AVDD_VOLTS;

  // --- Affichage format "table" (pratique pour Serial Plotter / Excel) ---
  Serial.print(E_raw);
  Serial.print('\t');
  Serial.print(e_mV, 4);
  Serial.print('\t');
  Serial.println(e_over_Vref, 10);

  delay(50);
}

// ============================================================
// Lecture HX711 – canal A – gain 128 (25 impulsions au total)
// ============================================================
int32_t readHX711_A128() {
  uint32_t data = 0;

  // Attente : donnée prête (DT = 0)
  while (digitalRead(DOUT_PIN) == HIGH) {}

  // Lecture des 24 bits (MSB en premier)
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(SCK_PIN, HIGH);
    data = (data << 1)   (uint32_t)digitalRead(DOUT_PIN);
    digitalWrite(SCK_PIN, LOW);
  }

  // 25e impulsion : sélection CANAL A, GAIN 128 pour la prochaine conversion
  digitalWrite(SCK_PIN, HIGH);
  digitalWrite(SCK_PIN, LOW);

  // Extension du signe (24 bits -> 32 bits)
  if (data & 0x800000UL) {        // bit 23 = 1 => nombre négatif
    data |= 0xFF000000UL;         // on remplit les bits 24..31 à 1
  }

  return (int32_t)data;
}
```
<br>
<br>
<br>

# Module limiteur de couple
<br>

<img
  src="./Assets/Module%20limiteur%20de%20couple/Module%20complet.jpg"
  alt="Module limiteur"
  width="30%"
/>


## Objectif:
<br>

### Vérifier la véracité du modèle local de Coulomb des frottements pour le couple transmissible

<br>

## Principe:

<br>

### Le principe du module est de pouvoir entre deux plaques de titanes dont l'une fixée au bâti appliquer un couple (grâce au support) et un effort presseur par compression d'un ressort pour trouver une relation entre le couple transmissible et cette force.

<br>

### Contrairement au schéma le ressort est autour de l'axe de rotation pour se rapprocher d'une pression uniformément répartie sur les surfaces.
<img
  src="./Assets/Module%20limiteur%20de%20couple/Principe%20limiteur.jpg"
  alt="Schema principe"
  width="30%"
/>


## Conception du module :

### Le module a été réalisé par soudure à l'arc à l'électrode enrobée et par tournage (pour la tige filetée).



<img
  src="./Assets/Module%20limiteur%20de%20couple/Soudage%20à%20l'arc.jpg"
  alt="Soudage"
  width="30%"
/>

<img
  src="./Assets/Module%20limiteur%20de%20couple/tournage.jpg"
  alt="Tournage"
  width="30%"
/>

## Montage final

### Une fois fixé sur le support on peut régler l'effort presseur en tournant l'écrou ce qui comprime le ressort .

<img
  src="./Assets/Module%20limiteur%20de%20couple/Module%20installé%20sans%20bras%20de%20levier.jpg"
  alt="Module installé"
  width="30%"
/>