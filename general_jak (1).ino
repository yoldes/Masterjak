
#include <Adafruit_NeoPixel.h> //Bibliothèque pour le ruban de LEDs
#define PIN 6        // Le ruban de LEDs est branché sur le pin n°6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(96, PIN, NEO_GRB + NEO_KHZ800); 

int joypin = A1;   // Le joystick est branché sur le pin analogique n°A1
int valuestick= 0;  //Valeur renvoyée par le joystick

// Les boutons couleurs sont branchés sur les pins 7 à 12 dans l'ordre suivant : rouge, vert, bleu, orange, violet, jaune
int buttonred= 7;    
int buttongreen= 8;
int buttonblue= 9;
int buttonorange= 10 ;
int buttonviolet= 11 ;
int buttonyellow= 12;

// Pour valider une ligne, on appuie sur le bouton 'start' branché dans la pin 13
int buttonvalid=13;

//Matrice contenant les couleurs dans l'ordre RVBOVJ
int array_couleurs[8][3]={{111,0,0},{0,111,0},{0,0,111},{111,25,0},{111,0,90},{111,50,0},{111,111,111},{0,0,0}};

//Etat des boutons:
int buttonrstate=0;
int buttongstate=0;
int buttonbstate=0;
int buttonostate=0;
int buttonvstate=0;
int buttonystate=0;
int buttonvalidstate=0;

//Numéro de la LED
int i =0; 

//Numéro du rang (0,4,8...)
int k=0;  

//Couleur des LEDs à chaque tentative:
int led1=7;  //Couleur de la led 1/4
int led2=7;  //Couleur de la led 2/4
int led3=7;  //Couleur de la led 3/4
int led4=7;  //Couleur de la led 4/4

//Numéro de la couleur dans la matrice array_couleurs
int f=-1;    

//Liste contenant le code à découvrir 
int comb[4]={0,0,0,0};

//Compteurs pour l'allumage des LEDs de vérification :
int correct=0;
int bienplace=0;

//Liste contenant les couleurs de chaque tentative :
int array_tentative[4]={led1,led2,led3,led4};

//Paramètres de la musique de fin:
const int buzzerPin = 5;  
const int songLength = 26;  // Nombre de notes dans la musique
char notes[songLength] = {'g', 'g', 'b', 'a', 'f', 'g', 'g', 'b', 'd', 'b', 'c', 'd', 'd', 'c','c', 'b', 'a', 'f', 'a','g','g','b','d','b','g'};
//Notes de do à si en notation anglaise (a,b,c..). Mettre un espace pour une pause 
// beats[] : liste contenant la durée de chaqua note. Un "3" represente une demie croche, "2" une croche, "1" une blanche. Attention les silences doivent avoir une durée aussi.
int beats[songLength] = {7,4,5,5,4,4,2,2,2,2,8,4,2,2,4,4,5,5,4,4,2,2,2,2,8,1};
int tempo = 70;  // Le tempo en battement par minutes

bool x=true; // Pour sortir de la boucle while de la musique 

//Code() définit le code secret à deviner
int code(){
   int r=random(6); //On tire quatre valeur au choix entre 0 et 5 (couleurs)
   int s=random(6);
   int t=random(6);
   int u=random(6);
  
  while((r==s)||(r==t)||(r==u)||(s==t)||(s==u)||(t==u)){ //Cette boucle permet d'avoir quatre couleurs différentes
   r=random(6);
   s=random(6);
   t=random(6);
   u=random(6);
   }
   
   comb[0]=r; //On attribue chaque valeur à la matrice comb (combinaison à deviner) on a donc qqch de la forme comb={0,5,3,4}
   comb[1]=s;
   comb[2]=t;
   comb[3]=u;
  //for (int k=11;k<15; k++){
  // strip.setPixelColor(k,array_couleurs[comb[k-11]][0],array_couleurs[comb[k-11]][1],array_couleurs[comb[k-11]][2]);
  //}
  return comb;
}


void setup() {
  
  //Paramètrage des boutons, buzzer :
  pinMode(buttonred, INPUT);
  pinMode(buttongreen, INPUT);
  pinMode(buttonblue, INPUT);
  pinMode(buttonorange, INPUT);
  pinMode(buttonviolet, INPUT);
  pinMode(buttonyellow, INPUT);
  pinMode(buttonvalid, INPUT);
  pinMode(buzzerPin, OUTPUT); 

  //Paramètrage des rubans (bibliothèque)
  strip.begin();
  strip.show();

  //Paramètrage de la bibliothèque des prints
  Serial.begin(9600);

  //Random ne renvoie pas vraiment une valeur aléatoire, il faut l'y forcer
  randomSeed(analogRead(1));

  //On appelle code() au démarrage
  code();
  
// Pour que la première LED s'allume en blanc (savoir où est le joystick)
  strip.setPixelColor(0,223,223,223);
  strip.show();
}

void loop() {                 
  masterjak();
  playagain();   

  }

 void masterjak(){ //code principal
  while((bienplace<4)&&(k<48)){  //Conditions pour que l'on reste dans la boucle de jeu : nb de LED bien placées <4 et encore des tentatives restantes
    
      choiceled(); //Fonction qui commande le joystick et par conséquent la LED sur laquelle on se trouve

      //Fonctions commandant les couleurs
      red();
      green();
      blue();
      orange();
      violet();
      yellow();

      //Fonction qui passe au rang suivant et allume les LEDs de vérification
      valid();

      //Allumage des LEDs :
      strip.show();

      //Attribution des couleurs dans la matrice array_tentative[]
      array_tentative[0]=led1;
      array_tentative[1]=led2;
      array_tentative[2]=led3;
      array_tentative[3]=led4;
  }
  delay(100);

  //Défaite
  if ((k==48)&&(bienplace!=4)){  
    for (int i=92;i<96;i++){
      strip.setPixelColor(i,array_couleurs[comb[i-92]][0],array_couleurs[comb[i-92]][1],array_couleurs[comb[i-92]][2]);
      strip.show();
    }
  }
  
  //Victoire 
  else if (bienplace==4){
  music(); //Appelle la fonction musique lorsque l'on sort de la boucle (gagné !)
  playagain();    //Appelle de la fonction pour rejouer après avoir gagné
 }
  }
  

//Choiceled : Fonction permettant de connaitre la position du joystick sur le ruban de LEDs
int choiceled(){
  valuestick= analogRead(joypin); //Lis les valeurs renvoyées par le joystick
  delay(100);

  //Mouvement à droite :
  if (valuestick>980){
    i=i+1;
     if (i<=k+3){
    strip.setPixelColor(i,223,223,223); //La LED sur laquelle on se trouve s'allume en blanc

    //Il faut récupérer l'information 'quelle couleur était sur la LED à gauche' 
    int e=(i-1)%4; //Sur quelle LED du ruban de 4 se trouve-t-on ? si e=0 : led1, si e=1 : led2, etc
    if (e==0){
      f=led1;
      strip.setPixelColor(i-1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==1){
      f=led2;
      strip.setPixelColor(i-1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==2){
      f=led3;
      strip.setPixelColor(i-1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==3){
      f=led4;
      strip.setPixelColor(i-1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    } 
    return i;
    strip.show();
  }

//Meme chose pour la gauche
  }else{
    if (valuestick<70){
      i=i-1;
      if (i>=k){
      strip.setPixelColor(i,223,223,223);
      int e=(i+1)%4;
    if (e==0){
      f=led1;
      strip.setPixelColor(i+1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==1){
      f=led2;
      strip.setPixelColor(i+1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==2){
      f=led3;
      strip.setPixelColor(i+1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    }else if(e==3){
      f=led4;
      strip.setPixelColor(i+1,array_couleurs[f][0],array_couleurs[f][1],array_couleurs[f][2]);
    } 
    return i;
      strip.show();
    }
  }
  }

//Limitation du joystick entre la LED 1 et 4;
if (i<k){
  i=k;
}else{
  if(i>k+3){
    i=k+3;
  }
}
}

//COULEURS

//Rouge
int red(){
  buttonrstate=digitalRead(buttonred); //Lit l'état du bouton de la couleur
  if (buttonrstate==LOW ){
    strip.setPixelColor(i,array_couleurs[0][0],array_couleurs[0][1],array_couleurs[0][2]); //On associe à la couleur rouge (n°0)
    int e=i%4; //Sur quelle LED est-on ?
    if (e==0){ //Si LED1 : associer la couleur (0 pour rouge), etc 
      led1=0;
      return led1;
    }else if(e==1){
      led2=0;
      return led2; 
    }else if(e==2){
      led3=0;
      return led3;
    }else if(e==3){
      led4=0;
      return led4;
    
  }
  
}
}

//Vert
int green(){
  buttongstate=digitalRead(buttongreen);
  if (buttongstate==LOW ){
    strip.setPixelColor(i,array_couleurs[1][0],array_couleurs[1][1],array_couleurs[1][2]);
        int e= i%4;
    if (e==0){
      led1=1;
      return led1;
    }else if(e==1){
      led2=1;
      return led2; 
    }else if(e==2){
      led3=1;
      return led3;
    }else if(e==3){
      led4=1;
      return led4;
  }
}
}

//Bleu
int blue(){
  buttonbstate=digitalRead(buttonblue);
  if (buttonbstate==LOW ){
    strip.setPixelColor(i,array_couleurs[2][0],array_couleurs[2][1],array_couleurs[2][2]);
        int e= i%4;
    if (e==0){
      led1=2;
      return led1;
    }else if(e==1){
      led2=2;
      return led2; 
    }else if(e==2){
      led3=2;
      return led3;
    }else if(e==3){
      led4=2;
      return led4;
  }
}
}

//Orange
int orange(){
  buttonostate=digitalRead(buttonorange);
  if (buttonostate==LOW ){
    strip.setPixelColor(i,array_couleurs[3][0],array_couleurs[3][1],array_couleurs[3][2]);
        int e= i%4;
    if (e==0){
      led1=3;
      return led1;
    }else if(e==1){
      led2=3;
      return led2; 
    }else if(e==2){
      led3=3;
      return led3;
    }else if(e==3){
      led4=3;
      return led4;
  }
}
}

//Violet
int violet(){
  buttonvstate=digitalRead(buttonviolet);
  if (buttonvstate==LOW ){
    strip.setPixelColor(i,array_couleurs[4][0],array_couleurs[4][1],array_couleurs[4][2]);
        int e= i%4;
    if (e==0){
      led1=4;
      return led1;
    }else if(e==1){
      led2=4;
      return led2; 
    }else if(e==2){
      led3=4;
      return led3;
    }else if(e==3){
      led4=4;
      return led4;
  }
}
}

//Jaune
int yellow(){
  buttonystate=digitalRead(buttonyellow);
  if (buttonystate==LOW){
    strip.setPixelColor(i,array_couleurs[5][0],array_couleurs[5][1],array_couleurs[5][2]);
    int e= i%4;
    if (e==0){
      led1=5;
      return led1 ;
    }else if(e==1){
      led2=5;
      return led2; 
    }else if(e==2){
      led3=5;
      return led3;
    }else if(e==3){
      led4=5;
      return led4;
  }
}
}
 
void music(){ //Fonction musique
 
  int i, duration; 
  for (i = 0; i < songLength; i++) {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // Si c'est une pause, pause
      delay(duration);            
    else                          // Sinon, jouer la note
    {
      tone(buzzerPin, frequency(notes[i]), duration);
    delay(duration);            // Attendre que la durée de la note soit écoulée
    }
    delay(tempo/10);              // petite pause entre les notes pour la lisibilité
  }
  while(x==true){
  // Comme on ne joue la chanson qu'une fois, on lui dit de s'arreter 'pour toujours'
  buttonvalidstate=digitalRead(buttonvalid);
  if (buttonvalidstate==LOW){
    x=false;
  }
  }
  playagain();
}

int frequency(char note) //Fonction commandant les fréquences des notes utilisées
{
  int i;
  const int numNotes = 8;  // Nombre de notes différentes que l'on souhaite avoir
  char names[numNotes] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'    }; //Nom des notes
  int frequencies[numNotes] = {
    523.25, 587.33, 622.25, 349.23, 392.00, 440, 466.16, 523.25    }; //Fréquence des notes en Hz
    
  for (i = 0; i < numNotes; i++)  // Parcours les notes une par une
  {
    if (names[i] == note)    
    {
      return(frequencies[i]);     //retourne la fréquence associée à la note
    }
  }
return(0);
}

//Fonction qui commande la validation de chaque tentative
void valid(){ 
  buttonvalidstate= digitalRead(buttonvalid); //Lit l'état du bouton valider
  if (buttonvalidstate==LOW){
    for(int i=0;i<4;i++){
      if((array_tentative[i]==6)||(array_tentative[i]==7)){ //On ne peut pas passer à la ligne suivante si il y a une LED noire ou blanche
        masterjak();
        }}
        delay(100);
      
    
    for (int y=0;y<4;y++){
      strip.setPixelColor(k+y,array_couleurs[array_tentative[y]][0],array_couleurs[array_tentative[y]][1],array_couleurs[array_tentative[y]][2]); //lorsqu'une led colorée devient blanche à cause du stick, la valeur de sa couleur n'est pas modifiée, ainsi la condition pour valider n'est pas entravée, elle s'effectue alors avec une led blanche, on allume à nouveau le ruban avec la combinaison qui elle est restée intacte.
    }
    
    k=k+4; //On passe au rang suivant
    i=k;   //On réinitialise i à la valeur de k
    if(k<48){
    strip.setPixelColor(k,111,111,111); //La premiere LED du nouveau rang s'allume en blanc (position du joystick)
    }
    
    //On initialise les valeurs des leds correctes et bien placées
    bienplace=0; 
    correct=0;

    //On appelle la fonction de vérification qui renvoie les nombre de led bien placées et correctes
    verification();
    

    //On initialise les LEDs pour qu'elles restent éteintes jusqu'à temps qu'on leur associe une couleur
    led1=7;
    led2=7;
    led3=7;
    led4=7;
  }
}


//Fonction pour compter les LEDs bien placées et correctes 
void verification(){
  for (int k=0; k<4;k++){
    if (array_tentative[k]==comb[k]){ //Si la valeur k de la combinaison et de la tentative est la même (même couleur)
      bienplace=bienplace+1;
  }
     for (int x=0;x<4;x++){
       if(array_tentative[x]==comb[k]){ //Si la valeur k de la combinaison et x de la tentative est la même 
         correct=correct+1;
         }
        }
      }
correct=correct-bienplace;

//On allume les LEDs de vérification
for (int x=0;x<bienplace;x++){
  strip.setPixelColor(x+k+44,50,111,0); //Vert
}
for (int x=0;x<correct;x++){
  strip.setPixelColor(x+44+k+bienplace,75,50,0); //Jaune
    }
    }
  

//Fonction rejouer
void playagain(){
  buttonvalidstate= digitalRead(buttonvalid);
  if (buttonvalidstate==LOW){
    delay(100);

    //On réinitialise les variables
    k=0; 
    i=0;
    bienplace=0;
    correct=0;

    //On éteint toutes les LEDs 
    for (int h=0;h<97;h++){
      strip.setPixelColor(h,0,0,0);
    }

    //On rappelle les fonctions code(), pour créer un nouveau code secret, et masterjak() pour relancer le code principal.
    strip.setPixelColor(0,111,111,111); //On rallume la toute première led pour que le joueur comprenne le début d'une nouvelle partie.
    code();
    
    masterjak();
    
  }
}
    
