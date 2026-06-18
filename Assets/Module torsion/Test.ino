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
// (c'est ce qui donne ~±20mV si AVDD=5V et GAIN=128)
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
    data = (data << 1) | (uint32_t)digitalRead(DOUT_PIN);
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
