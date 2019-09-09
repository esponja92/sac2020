struct _mensagem {
  char label[20];
  int valor;
};

typedef struct _mensagem mensagem;

static int hr[101] = {72, 72, 72, 72, 72, 72, 72, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71};

static int resp[101] = {25, 25, 25, 25, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 17, 20, 24, 24, 24, 24, 25, 25, 25, 25, 23, 23, 23, 28, 28, 28, 31, 30, 29, 28, 29, 29, 29, 29, 26, 26, 26, 26, 26, 26, 23, 23, 23, 23, 20, 22, 21, 20, 19, 18, 19, 19, 18, 18, 19, 19, 18, 21, 21, 25, 25, 25, 25, 25, 25, 25, 24, 23, 29, 31, 30, 29, 28, 25, 25, 25, 25, 25, 22, 22, 22, 21, 21, 23, 23, 23, 25};

static int bp[101] = {129, 129, 129, 129, 129, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 127, 127, 127, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 125, 125, 125, 125, 125, 125, 125, 125, 126, 125, 125, 125, 125, 126, 125, 125, 125, 125, 125, 124, 124, 124, 124, 123, 123, 123, 122, 123, 122, 122, 122, 122, 122, 122, 122, 122, 123, 123, 123, 123, 123, 124, 124, 124, 124, 123, 124, 123, 123, 123, 123, 124, 124, 124};

static int spo2[101] = {91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91};
