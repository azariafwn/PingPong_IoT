#include <MD_Parola.h> 
#include <MD_MAX72xx.h> 
#include <SPI.h> 
// Konfigurasi hardware MAX7219 
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4 
#define CS_PIN 5 
#define DIN_PIN 23 
MD_MAX72XX Display(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); 
MD_Parola DisplayParola = MD_Parola(HARDWARE_TYPE, CS_PIN, 
MAX_DEVICES); 
// Konfigurasi pin untuk kontrol 
#define POT1_PIN 35 // Potensiometer Player 1 
#define POT2_PIN 34 // Potensiometer Player 2 
#define SMASH1_PIN 32 // Tombol Smash Player 1 
#define SMASH2_PIN 33 // Tombol Smash Player 2 
// Variabel game 
int paddle1Pos = 3, paddle2Pos = 3; // Posisi paddle 
int ballX = 16, ballY = 4;          
// Posisi bola 
int ballDirX = -1, ballDirY = 1;    
int score1 = 0, score2 = 0;         
const int maxScore = 5;             
int ballSpeed = 200;                
const int normalBallSpeed = 200;    
bool gameStarted = false;       
// Arah bola 
// Skor 
// Skor maksimum 
// Kecepatan bola saat ini 
// Kecepatan bola normal 
// Flag untuk memulai permainan 
int lastScorer = 0;             
cetak skor (1 atau 2) 
// Menyimpan pemain terakhir yang 
bool ballAttached = false;  // Menentukan apakah bola menempel pada 
paddle 
unsigned long smashStartTime = 0; // Waktu mulai smash 
const int smashDuration = 500;   // Durasi efek smash dalam milidetik 
// Fungsi untuk teks berjalan menggunakan Parola 
void showRunningText(String text) { 
DisplayParola.begin(); // Memulai Parola 
DisplayParola.displayText( 
text.c_str(), 
PA_CENTER,      
// Posisi teks di tengah 
100,            
1000,           
// Kecepatan scroll 
// Waktu tunggu sebelum scroll ulang 
PA_SCROLL_LEFT  // Efek scroll kiri 
); 
 
  while (!DisplayParola.displayAnimate()) { 
    // Animasi berjalan hingga selesai 
  } 
 
  // Tidak perlu reset(), cukup set mode IDLE 
  DisplayParola.displayReset();  
} 
 
// Fungsi untuk menggambar elemen di dot matrix 
void drawPixel(int x, int y, bool state) { 
  if (x >= 0 && x < MAX_DEVICES * 8 && y >= 0 && y < 8) { // Validasi 
    Display.setPoint(y, x, state); 
  } 
} 
 
// Fungsi untuk menampilkan skor pemain 
void showScore() { 
  Display.clear(); 
  String scoreText = String(score1) + ":" + String(score2); 
 
  for (int i = 0; i < scoreText.length(); i++) { 
    Display.setChar(i * 8 + 8, scoreText[i]); // Geser sedikit lebih 
ke kiri 
  } 
 
  Display.update(); 
  delay(2000); // Tampilkan selama 2 detik 
} 
 
// Fungsi untuk menggambar elemen permainan 
void updateDisplay() { 
  Display.clear(); 
 
  // Tampilkan skor Player 1 (di sisi kiri) 
  Display.setChar(5, '0' + score1); 
 
  // Tampilkan skor Player 2 (di sisi kanan) 
  Display.setChar(30, '0' + score2); 
 
  // Gambar paddle player 1 (sisi kiri) 
  for (int i = -1; i <= 1; i++) { 
    int y = paddle1Pos + i; 
    if (y >= 0 && y < 8) { 
      drawPixel(8, y, true); // Paddle kiri berada di kolom 8 
    } 
  } 
 
  // Gambar paddle player 2 (sisi kanan) 
  for (int i = -1; i <= 1; i++) { 
    int y = paddle2Pos + i; 
    if (y >= 0 && y < 8) { 
      drawPixel(23, y, true); // Paddle kanan berada di kolom 23 
    } 
  } 
 
  // Gambar bola 
  drawPixel(ballX, ballY, true); 
} 
 
// Start permainan saat tombol ditekan 
void checkStartGame() { 
  if (!gameStarted && (digitalRead(SMASH1_PIN) == LOW || 
digitalRead(SMASH2_PIN) == LOW)) { 
    gameStarted = true; 
    ballDirX = (digitalRead(SMASH1_PIN) == LOW) ? 1 : -1; // Arahkan 
bola sesuai tombol 
    ballDirY = (random(0, 2) == 0) ? -1 : 1; 
  } 
} 
 
// Fungsi untuk update posisi bola 
// Tambahkan logika ini dalam fungsi `updateGame` untuk memperbaiki 
smash 
void updateGame() { 
  checkStartGame(); // Cek tombol start 
  if (!gameStarted) { 
    updateDisplay(); 
    return; 
  } 
 
  // Tambahkan logika saat bola menempel di paddle setelah servis 
  if (ballAttached) { 
    if (lastScorer == 1 && digitalRead(SMASH1_PIN) == LOW) { 
      ballAttached = false; // Lepaskan bola 
      ballDirX = 1; // Arah bola ke kanan 
      ballDirY = (random(0, 2) == 0) ? -1 : 1; // Random arah vertikal 
    } else if (lastScorer == 2 && digitalRead(SMASH2_PIN) == LOW) { 
      ballAttached = false; // Lepaskan bola 
      ballDirX = -1; // Arah bola ke kiri 
      ballDirY = (random(0, 2) == 0) ? -1 : 1; // Random arah vertikal 
    } 
    updateDisplay(); // Perbarui tampilan 
    return; // Kembalikan agar bola tidak bergerak sebelum dilepaskan 
  } 
 
  // Update posisi paddle berdasarkan potensiometer 
  paddle1Pos = map(analogRead(POT1_PIN), 0, 4095, 1, 6); 
  paddle2Pos = map(analogRead(POT2_PIN), 0, 4095, 1, 6); 
 
  // Reset kecepatan setelah durasi smash berakhir 
  if (millis() - smashStartTime > smashDuration) { 
    ballSpeed = normalBallSpeed; // Kembali ke kecepatan normal 
  } 
 
  // Periksa smash untuk setiap paddle 
  if ((ballX == 9 && ballY >= paddle1Pos - 1 && ballY <= paddle1Pos + 
1) && digitalRead(SMASH1_PIN) == LOW) { 
    ballSpeed = 75; // Tingkatkan kecepatan bola 
    smashStartTime = millis(); // Catat waktu smash 
    ballDirX = 1; // Pastikan bola bergerak ke kanan 
  } 
 
  if ((ballX == 22 && ballY >= paddle2Pos - 1 && ballY <= paddle2Pos + 
1) && digitalRead(SMASH2_PIN) == LOW) { 
    ballSpeed = 75; // Tingkatkan kecepatan bola 
    smashStartTime = millis(); // Catat waktu smash 
    ballDirX = -1; // Pastikan bola bergerak ke kiri 
  } 
 
  // Update posisi bola 
  ballX += ballDirX; 
  ballY += ballDirY; 
 
  // Pantulan bola pada dinding atas/bawah 
  if (ballY <= 0) ballDirY = 1; 
  if (ballY >= 7) ballDirY = -1; 
 
  // Pantulan bola pada paddle player 1 
  if (ballX == 9 && ballY >= paddle1Pos - 1 && ballY <= paddle1Pos + 
1) { 
    ballDirX = 1; 
  } 
 
  // Pantulan bola pada paddle player 2 
  if (ballX == 22 && ballY >= paddle2Pos - 1 && ballY <= paddle2Pos + 
1) { 
    ballDirX = -1; 
  } 
 
  // Jika bola melewati sisi kiri 
  if (ballX < 8) { 
    score2++; 
    lastScorer = 2; // Pemain 2 mencetak skor 
    checkWinner(); 
    resetBall(); 
    delay(800); 
  } 
 
  // Jika bola melewati sisi kanan 
  if (ballX > 23) { 
    score1++; 
    lastScorer = 1; // Pemain 1 mencetak skor 
    checkWinner(); 
    resetBall(); 
    delay(800); 
  } 
 
  updateDisplay(); 
} 
 
// Reset posisi bola (jangan ubah kecepatan bola selama servis awal) 
void resetBall() { 
  ballAttached = true;  // Bola menempel saat servis 
  ballSpeed = normalBallSpeed; // Kembali ke kecepatan normal 
 
  if (lastScorer == 1) { 
    ballX = 9;  // Posisi bola dekat paddle Player 1 
    ballY = paddle1Pos;  // Sesuaikan dengan posisi paddle Player 1 
  } else if (lastScorer == 2) { 
    ballX = 22;  // Posisi bola dekat paddle Player 2 
    ballY = paddle2Pos;  // Sesuaikan dengan posisi paddle Player 2 
  } 
} 
 
 
// Fungsi untuk mengecek pemenang 
void checkWinner() { 
  if (score1 >= maxScore) { 
    showRunningText("Player 1 Wins! "); // Tampilkan running text 
    resetGame(); 
  } else if (score2 >= maxScore) { 
    showRunningText("Player 2 Wins! "); // Tampilkan running text 
    resetGame(); 
  } 
} 
 
// Reset permainan 
void resetGame() { 
  score1 = 0; 
  score2 = 0; 
  resetBall(); 
} 
 
void setup() { 
  Display.begin(); 
  Display.control(MD_MAX72XX::INTENSITY, 5); 
 
  DisplayParola.begin(); // Inisialisasi Parola 
  showRunningText("AZARIA PUTRI FAWNIA - 5024221038 - PINGPONG "); 
 
  pinMode(POT1_PIN, INPUT); 
  pinMode(POT2_PIN, INPUT); 
  pinMode(SMASH1_PIN, INPUT_PULLUP); 
  pinMode(SMASH2_PIN, INPUT_PULLUP); 
  randomSeed(analogRead(0)); 
 
  Serial.begin(115200); // Inisialisasi komunikasi Serial 
  Serial.println("Debugging Button Input..."); 
  // Tampilkan teks berjalan di awal 
 
} 
 
void loop() { 
 
  int smash1State = digitalRead(SMASH1_PIN); 
  int smash2State = digitalRead(SMASH2_PIN); 
 
  // Cetak ke Serial Monitor 
  Serial.print(" | SMASH1_PIN: "); 
  Serial.print(smash1State == LOW ? "DITEKAN " : "TIDAK "); 
  Serial.print(" | SMASH2_PIN: "); 
  Serial.println(smash2State == LOW ? "DITEKAN" : "TIDAK"); 
 
  Serial.println(("| ball speed : "));; 
  Serial.print(ballSpeed); 
 
  updateGame(); 
  delay(ballSpeed); 
} 
