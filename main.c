#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

struct Car {
    char *km;
    char *type;
    char *transmission;
    char *stockNumber;
    char *driveTrain;
    char *status;
    char *fuelEconomy;
    char *year;
    char *trim;
    char *setOfFeatures;
    struct Car *next;
};


struct Model {
    char name[256];
    struct Model *next;
    struct Car *below;
};

struct Maker {
    char name[32];
    struct Maker *next;
    struct Model *below;
};

struct Maker *catalogue = NULL;

char *readLine(FILE *file) {
    int ch;
    int i = 0;
    size_t bufferLength = 1;
    char *buffer = malloc(bufferLength);

    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        buffer = realloc(buffer, ++bufferLength);
        buffer[i++] = (char) ch;
    }
    buffer[i] = '\0';

    if (ch == EOF) {
        free(buffer);
        return NULL;
    }
    return buffer;
}

char **parseLine(char *line) {
    char *p = line;
    char *token;
    int tokenCount = 0;
    int i;
    size_t bufferLength;
    int bracket;
    char **tokens = malloc(sizeof(char *) * 12);

    while (*p != '\0') {
        if (*p == ',') {
            token = malloc(1);
            *token = '\0';
            p++;
        } else {
            bracket = 0;
            bufferLength = 1;
            i = 0;
            if (*p == '{') {
                bracket = 1;
                p++;
            }
            token = malloc(bufferLength);
            token[i++] = *p++;
            while (*p != '\0' && (bracket || *p != ',') && (!bracket || *p != '}')) {
                token = realloc(token, ++bufferLength);
                token[i++] = *p++;
            }
            if (*p == '}') {
                p++;
            }
            token = realloc(token, ++bufferLength);
            token[i] = '\0';
            tokens[tokenCount++] = token;
            if (*p == ',') {
                p++;
            }
        }
    }
    return tokens;
}

struct Maker *addOrGetMaker(char *makerName) {
    struct Maker *m = NULL;
    struct Maker *pm = NULL;

    if (catalogue == NULL) {
        catalogue = malloc(sizeof(struct Maker));
        strcpy((*catalogue).name, makerName);
        catalogue->below = NULL;
        catalogue->next = NULL;
        return catalogue;
    } else {
        pm = m = catalogue;
        while (m != NULL) {
            pm = m;
            if (strcmp(makerName, m->name) == 0) {
                return m;
            }
            m = m->next;
        }
        m = malloc(sizeof(struct Maker));
        strcpy((*m).name, makerName);
        m->below = NULL;
        m->next = NULL;
        pm->next = m;
        return m;
    }
}

struct Model *addOrGetModel(struct Maker *currentMaker, char *modelName) {
    struct Model *m = currentMaker->below;
    struct Model *pm = currentMaker->below;

    if (m == NULL) {
        m = malloc(sizeof(struct Model));
        strcpy((*m).name, modelName);
        m->below = NULL;
        m->next = NULL;
        currentMaker->below = m;
    } else {
        while (m != NULL) {
            pm = m;
            if (strcmp(modelName, m->name) == 0) {
                return m;
            }
            m = m->next;
        }
        m = malloc(sizeof(struct Maker));
        strcpy((*m).name, modelName);
        m->below = NULL;
        m->next = NULL;
        pm->next = m;
    }
    return m;
}

void addCar(struct Model *model, char **tokens) {
    int tokenIndex = 2;

    struct Car *car = malloc(sizeof(struct Model));

    car->trim = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->trim, tokens[tokenIndex++]);

    car->km = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->km, tokens[tokenIndex++]);

    car->year = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->year, tokens[tokenIndex++]);

    car->type = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->type, tokens[tokenIndex++]);

    car->driveTrain = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->driveTrain, tokens[tokenIndex++]);

    car->transmission = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->transmission, tokens[tokenIndex++]);

    car->stockNumber = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->stockNumber, tokens[tokenIndex++]);

    car->status = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->status, tokens[tokenIndex++]);

    car->fuelEconomy = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->fuelEconomy, tokens[tokenIndex++]);

    car->setOfFeatures = malloc(strlen(tokens[tokenIndex]));
    strcpy(car->setOfFeatures, tokens[tokenIndex]);

    if (model->below == NULL) {
        model->below = car;
    } else {
        struct Car *c = model->below;
        struct Car *lastCar = car;
        while (c->next != NULL) {
            lastCar = c;
            c = c->next;
        }
        lastCar->next = car;
    }
}

void add2Inventory(char *line) {
    char *makerName;
    char *modelName;
    char **tokens;
    int i;

    tokens = parseLine(line);

    makerName = tokens[0];
    struct Maker *currentMaker = addOrGetMaker(makerName);
    modelName = tokens[1];
    struct Model *currentModel = addOrGetModel(currentMaker, modelName);

    addCar(currentModel, tokens);

    free(line);

    for (i = 0; i < 12; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

void saveCatalogue2File() {
    FILE *file;
    struct Maker *maker = catalogue;

    file = fopen("/Users/olivierfradette-roy/CLionProjects/untitled2/OrderedCars2.txt", "w");

    while (maker != NULL) {
        fprintf(file, "%s,", maker->name);
        struct Model *model = maker->below;
        while (model != NULL) {
            fprintf(file, "%s,", model->name);
            struct Car *car = model->below;
            while (car != NULL) {
                fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,{%s}\n", car->trim, car->km, car->year, car->type,
                        car->driveTrain, car->transmission, car->stockNumber, car->status,
                        car->fuelEconomy, car->setOfFeatures);
                car = car->next;
            }
            model = model->next;
        }
        maker = maker->next;
    }
    fclose(file);
}

void printCar(char *makerName, char *modelName, struct Car *car) {
    printf("car: %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,{%s}\n", makerName, modelName, car->trim,
           car->km, car->year, car->type,
           car->driveTrain, car->transmission, car->stockNumber, car->status,
           car->fuelEconomy, car->setOfFeatures);
}

void searchMaker(char *value) {
    struct Maker *maker = catalogue;

    while (maker != NULL) {
        if (strcmp(value, maker->name) == 0) {
            struct Model *model = maker->below;
            while (model != NULL) {
                struct Car *car = model->below;
                while (car != NULL) {
                    printCar(maker->name, model->name, car);
                    car = car->next;
                }
                model = model->next;
            }
        }
        maker = maker->next;
    }
}

void searchModel(char *value) {
    struct Maker *maker = catalogue;

    while (maker != NULL) {
        struct Model *model = maker->below;
        while (model != NULL) {
            if (strcmp(value, model->name) == 0) {
                struct Car *car = model->below;
                while (car != NULL) {
                    printCar(maker->name, model->name, car);
                    car = car->next;
                }
            }
            model = model->next;
        }
        maker = maker->next;
    }
}

void searchCar(char *key, char *value) {
    struct Maker *maker = catalogue;
    int match;

    while (maker != NULL) {
        struct Model *model = maker->below;
        while (model != NULL) {
            struct Car *car = model->below;
            while (car != NULL) {
                match = 0;
                if (strcmp("#km", key) == 0 && strcmp(car->km, value) == 0) {
                    match = 1;
                } else if (strcmp("#km", key) == 0 && strcmp(car->km, value) == 0) {
                    match = 1;
                } else if (strcmp("Type", key) == 0 && strcmp(car->type, value) == 0) {
                    match = 1;
                } else if (strcmp("Transmission", key) == 0 && strcmp(car->transmission, value) == 0) {
                    match = 1;
                } else if (strcmp("stock#", key) == 0 && strcmp(car->stockNumber, value) == 0) {
                    match = 1;
                } else if (strcmp("DriveTrain", key) == 0 && strcmp(car->driveTrain, value) == 0) {
                    match = 1;
                } else if (strcmp("Status", key) == 0 && strcmp(car->status, value) == 0) {
                    match = 1;
                } else if (strcmp("Fuel Economy", key) == 0 && strcmp(car->fuelEconomy, value) == 0) {
                    match = 1;
                } else if (strcmp("Year", key) == 0 && strcmp(car->year, value) == 0) {
                    match = 1;
                } else if (strcmp("Trim", key) == 0 && strcmp(car->trim, value) == 0) {
                    match = 1;
                } else if (strcmp("set_of_features", key) == 0 && strcmp(car->setOfFeatures, value) == 0) {
                    match = 1;
                }
                if (match) {
                    printCar(maker->name, model->name, car);
                }
                car = car->next;
            }
            model = model->next;
        }
        maker = maker->next;
    }
}

void searchInventory(char *key, char *value) {
    printf("searching for key: %s   value: %s\n", key, value);
    if (strcmp(key, "car_maker") == 0) {
        searchMaker(value);
    } else if (strcmp(key, "Model") == 0) {
        searchModel(value);
    } else {
        searchCar(key, value);
    }
}

void readFile() {
    FILE *file;
    char *line = NULL;

    file = fopen("/Users/olivierfradette-roy/CLionProjects/untitled2/OrderedCars2.txt", "r");

    line = readLine(file);
    while (line != NULL) {
        add2Inventory(line);
        line = readLine(file);
    }

    saveCatalogue2File();

    searchInventory("car_maker", "Toyota");
    searchInventory("Model", "CLK");
    searchInventory("#km", "65101km");
    searchInventory("Status", "Used");
}

int main() {
    readFile();
    return 0;
}
