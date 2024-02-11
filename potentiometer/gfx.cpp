
#include "gfx.h"

static bool redrawVol = true;
static bool redrawVolNumber = true;
static bool redrawInpt = true;
static bool redrawInptNumber = true;
static bool redrawVolBar = true;

static int lastL = P_UNDETERMINED;
static int lastR = P_UNDETERMINED;

static bool redrawMute = false;
static bool muteState = false;
static bool muteDrawn = false;

static bool redrawError = false;
static bool errorDrawn = false;

static unsigned long muteTime = 0;

unsigned int loadingGradient[LOADING_STEPS];

void drawSmallKernelAcousticLogo(void) {
  int x, y;
  TFT *tft = returnTFTReference();

  x = (SCREEN_W - INTRO_KERNEL_ACOUSTIC_WIDTH) / 2;
  y = (SCREEN_H - INTRO_KERNEL_ACOUSTIC_HEIGHT) / 2;
  tft->fillRect(x, y, INTRO_KERNEL_ACOUSTIC_WIDTH,
                INTRO_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR);

  x = (SCREEN_W - LOAD_KERNEL_ACOUSTIC_WIDTH) / 2;
  y = ((SCREEN_H - LOAD_KERNEL_ACOUSTIC_HEIGHT) / 2) / 1.3;
  tft->drawImage(x, y, LOAD_KERNEL_ACOUSTIC_WIDTH,
                  LOAD_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)load_kernel_acoustic);
}

void showKernelAcousticLogo(void) {
  TFT *tft = returnTFTReference();
  clearScreen();
  const int x = (SCREEN_W - INTRO_KERNEL_ACOUSTIC_WIDTH) / 2;
  const int y = (SCREEN_H - INTRO_KERNEL_ACOUSTIC_HEIGHT) / 2;
  tft->drawImage(x, y, INTRO_KERNEL_ACOUSTIC_WIDTH,
                  INTRO_KERNEL_ACOUSTIC_HEIGHT, ICONS_BG_COLOR,
                  (unsigned short*)intro_kernel_acoustic);
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
  lastR = lastL = P_UNDETERMINED;
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

void drawMute(bool state) {
  redrawMute = true;
  muteState = state;
  clearScreen();
  muteDrawn = false;
  muteTime = millis() + (SECOND * MUTE_TIME_DISPLAY);
}

void clearError(void) {
  redrawError = errorDrawn = false;
  setupErrorDetection();
}

void drawError(void) {
  redrawError = true;
  errorDrawn = false;
}

bool isErrorActive(void) {
  return redrawError;
}

void drawLineSection(int x, int y, int sections, uint16_t color) {
  TFT *tft = returnTFTReference();

  for (int i = 0; i < sections; i++) {
    int x1 = x + i * VOLUME_BAR_OFFSET;
    tft->drawLine(x1, y, x1, y + VOL_BAR_HEIGHT, color);
  }
}

void drawVolumeBar(int x, int y, int val) {
  int area_0 = (val > SECTION_0) ? SECTION_0 : val;
  int area_1 = (val > SECTION_0) ? ((val - SECTION_0 > SECTION_1) ? SECTION_1 : val - SECTION_0) : 0;
  int area_2 = (val > SECTION_M) ? (val - SECTION_M) : 0;

  drawLineSection(x, y, area_0, vrgbToRgb565(VOL_MIN_ON));
  drawLineSection(x + area_0 * VOLUME_BAR_OFFSET, y, SECTION_0 - area_0, vrgbToRgb565(VOL_MIN_OFF));
  drawLineSection(x + SECTION_0 * VOLUME_BAR_OFFSET, y, area_1, vrgbToRgb565(VOL_MIDDLE_ON));
  drawLineSection(x + (SECTION_0 + area_1) * VOLUME_BAR_OFFSET, y, SECTION_1 - area_1, vrgbToRgb565(VOL_MIDDLE_OFF));
  drawLineSection(x + SECTION_M * VOLUME_BAR_OFFSET, y, area_2, vrgbToRgb565(VOL_MAX_ON));
  drawLineSection(x + (SECTION_M + area_2) * VOLUME_BAR_OFFSET, y, SECTION_2 - area_2, vrgbToRgb565(VOL_MAX_OFF));
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

  if(redrawMute) {
    if(muteTime < millis()) {
      redrawMute = false;
      redrawScreen();
    }

    if(!muteDrawn) {
      unsigned short *mute = NULL;
      int w = 0, h = 0;
      if(muteState) {
        x = (SCREEN_W - MUTE_WIDTH) / 2;
        y = (SCREEN_H - MUTE_HEIGHT) / 2;
        w = MUTE_WIDTH;
        h = MUTE_HEIGHT;
        mute = (unsigned short *)mute_g;
      } else {
        x = (SCREEN_W - MUTE_OFF_WIDTH) / 2;
        y = (SCREEN_H - MUTE_OFF_HEIGHT) / 2;
        w = MUTE_OFF_WIDTH;
        h = MUTE_OFF_HEIGHT;
        mute = (unsigned short *)mute_off_g;
      }
      tft->drawImage(x, y, w, h, ICONS_BG_COLOR,
                    (unsigned short*)mute);

      muteDrawn = true;
    }
    return true;
  }

  if(redrawError) {
    if(!errorDrawn) {
      clearScreen();

      x = (SCREEN_W - ERROR_WIDTH) / 2;
      y = (SCREEN_H - ERROR_HEIGHT) / 2;

      tft->drawImage(x, y, ERROR_WIDTH, ERROR_HEIGHT, ICONS_BG_COLOR,
                    (unsigned short*)error_g);
      errorDrawn = true;
    }
    return true;
  }

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
    x += VOL_DIGIT_WIDTH + VOL_DIGIT_OFFSET;

    if(lastR != r) {
      lastR = r;
      tft->drawImage(x, y, VOL_DIGIT_WIDTH, VOL_DIGIT_HEIGHT, ICONS_BG_COLOR,
                    (unsigned short*)vol_numbers[r]);
    }

    redrawVolNumber = false;
  }

  if(redrawVolBar) {
    drawVolumeBar(VOLUME_BAR_X, VOLUME_BAR_Y, values[V_VOLUME]);
    redrawVolBar = false;
  }

  return true;
}

