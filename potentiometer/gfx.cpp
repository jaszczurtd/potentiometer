
#include "gfx.h"

static bool redrawVol = true;
static bool redrawVolNumber = true;
static bool redrawInpt = true;
static bool redrawInptNumber = true;
static bool redrawVolBar = true;

static int lastL = -1;
static int lastR = -1;

unsigned int loadingGradient[LOADING_STEPS];

void drawSmallKernelAcousticLogo(void) {
  int x, y;
  TFT *tft = returnTFTReference();

  x = (SCREEN_W - WSTEP_KERNEL_ACOUSTIC_WIDTH) / 2;
  y = (SCREEN_H - WSTEP_KERNEL_ACOUSTIC_HEIGHT) / 2;
  tft->fillRect(x, y, WSTEP_KERNEL_ACOUSTIC_WIDTH,
                WSTEP_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR);

  x = (SCREEN_W - LOAD_KERNEL_ACOUSTIC_WIDTH) / 2;
  y = ((SCREEN_H - LOAD_KERNEL_ACOUSTIC_HEIGHT) / 2) / 1.3;
  tft->drawImage(x, y, LOAD_KERNEL_ACOUSTIC_WIDTH,
                  LOAD_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)load_kernel_acoustic);
}

void showKernelAcousticLogo(void) {
  TFT *tft = returnTFTReference();
  clearScreen();
  const int x = (SCREEN_W - WSTEP_KERNEL_ACOUSTIC_WIDTH) / 2;
  const int y = (SCREEN_H - WSTEP_KERNEL_ACOUSTIC_HEIGHT) / 2;
  tft->drawImage(x, y, WSTEP_KERNEL_ACOUSTIC_WIDTH,
                  WSTEP_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)wstep_kernel_acoustic);
}

void drawLoadingSequence(int amount) {
  int x, y;
  int a = 0;
  TFT *tft = returnTFTReference();

  if(amount >= AMOUNT_BARS_UNTIL_LOGO) {
    a = amount - 1;
  }

  y = LOADING_BAR_Y;
  for(; a < amount; a++) {
    x = LOADING_BAR_X + (a * LOADING_BAR_X_OFFSET);
    tft->drawLine(x, y, x, y + LOADING_BAR_HEIGHT, loadingGradient[a]);
  }

  if(amount <= AMOUNT_BARS_UNTIL_LOGO) {
    x = LOADING_X;
    y = LOADING_Y;
    tft->drawRGBBitmapTransparent(x, y, (unsigned short*)loading,
                                  LOADING_WIDTH, LOADING_HEIGHT, ICONS_BG_COLOR);
  }
}

void redrawScreen(void) {
  clearScreen();
  lastR = lastL = -1;
  redrawVol = true;
  redrawVolNumber = true;
  redrawInpt = true;
  redrawInptNumber = true;
  redrawVolBar = true;
}

void redrawVolume(void) {
  redrawVolNumber = true;
  redrawVolBar = true;
}

void redrawInput(void) {
  redrawInptNumber = true;
}

const unsigned short *vol_numbers[] = {
  vol_0, vol_1, vol_2, vol_3, vol_4, vol_5, vol_6, vol_7, vol_8, vol_9
};

bool displayValues(void *v) {

  if(!isPowerON()){
    return true;
  }
  if(!isSystemLoaded()) {
    return true;
  }
  TFT *tft = returnTFTReference();
  int x, y;

  if(redrawInpt) {
    x = INPUT_G_X;
    y = INPUT_G_Y;
    tft->drawImage(x, y, INPUT_G_WIDTH, INPUT_G_HEIGHT, ICONS_BG_COLOR,
                    (unsigned short*)input_g);
    redrawInpt = false;
  }

  if(redrawInptNumber) {
    const unsigned short *input_numbers[] = {
      input_d_1, input_d_2, input_d_3, input_d_4, input_d_5, input_d_6
    };
    
    x = INPUT_D_X;
    y = INPUT_D_Y;
    tft->drawImage(x, y, INPUT_D_WIDTH, INPUT_D_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)input_numbers[values[V_SELECTED_INPUT]]);
    redrawInptNumber = false;
  }

  if(redrawVol) {
    x = VOLUME_G_X;
    y = VOLUME_G_Y;
    tft->drawImage(x, y, VOLUME_G_WIDTH, VOLUME_G_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)volume_g);
    redrawVol = false;
  }

  if(redrawVolNumber) {
    x = VOL_DIGIT_X;
    y = VOL_DIGIT_Y;

    int l = values[V_VOLUME] / 10;
    if(lastL != l) {
      lastL = l;
      tft->drawImage(x, y, VOL_DIGIT_WIDTH, VOL_DIGIT_HEIGHT, ICONS_BG_COLOR,
                    (unsigned short*)vol_numbers[l]);
    }
    int r = values[V_VOLUME] % 10;
    x += VOL_DIGIT_WIDTH + 24;

    if(lastR != r) {
      lastR = r;
      tft->drawImage(x, y, VOL_DIGIT_WIDTH, VOL_DIGIT_HEIGHT, ICONS_BG_COLOR,
                    (unsigned short*)vol_numbers[r]);
    }

    redrawVolNumber = false;
  }

  return true;
}

