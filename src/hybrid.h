// 2014 - Jamie Hegarty - @Hegrz

#pragma once
#include "pebble.h"
#include "num2words.h"
#include "battbar.h"

  
#define BUFFER_SIZE 40
#define SAVEKEY_SETTINGS 1

// javascript keys
enum {
	KEY_SHOWDATE = 0,
	KEY_DATEFORMAT = 1,
	KEY_INVERT = 2
};

//Variables

Window *window;
TextLayer *hour_text_layer;
TextLayer *minute_text_layer;
TextLayer *date_text_layer;
InverterLayer *inverter_layer;

// settings structure
//
struct {
	int showdate;
	int dateformat;
	int invert;
	int old_showdate;
	int old_dateformat;
	int old_invert;
} settings;

void confUpdate();
void drawDate();
void drawInvert();
