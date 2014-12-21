#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" 
                           
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _rst 9
#define _dc 8

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

const String TWITTER_ACCESS_TOKEN = "xxxxx";
const String TWITTER_ACCESS_TOKEN_SECRET = "xxxxx";
const String TWITTER_CONSUMER_KEY = "xxxxx";
const String TWITTER_CONSUMER_SECRET = "xxxxx";

void setup() {
  tft.begin();
  tft.fillScreen(ILI9340_BLACK);
  tft.setRotation(1);
  Bridge.begin();
}
void loop()
{
    // Send to the Screen
    tft.fillScreen(ILI9340_BLACK);
    tft.setCursor(0, 0);

    TembooChoreo HomeTimelineChoreo;
    // invoke the Temboo client.
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    HomeTimelineChoreo.begin();
    // set Temboo account credentials
    HomeTimelineChoreo.setAccountName(TEMBOO_ACCOUNT);
    HomeTimelineChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    HomeTimelineChoreo.setAppKey(TEMBOO_APP_KEY);
    // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
    HomeTimelineChoreo.setChoreo("/Library/Twitter/Timelines/HomeTimeline");
    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
    // for complete details about the inputs for this Choreo
    HomeTimelineChoreo.addInput("Count", "1"); // the max number of Tweets to return from each request
    HomeTimelineChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    HomeTimelineChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    HomeTimelineChoreo.addInput("ConsumerKey", TWITTER_CONSUMER_KEY);    
    HomeTimelineChoreo.addInput("ConsumerSecret", TWITTER_CONSUMER_SECRET);
    // next, we'll define two output filters that let us specify the 
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters
    // we want the text of the tweet
    HomeTimelineChoreo.addOutputFilter("tweet", "/[1]/text", "Response");
    // and the name of the author
    HomeTimelineChoreo.addOutputFilter("author", "/[1]/user/screen_name", "Response");
    // tell the Process to run and wait for the results. The 
    // return code will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = HomeTimelineChoreo.run();
    // a response code of 0 means success; print the API response
    if(returnCode == 0) {
      String author; // a String to hold the tweet author's name
      String tweet; // a String to hold the text of the tweet
      // choreo outputs are returned as key/value pairs, delimited with 
      // newlines and record/field terminator characters, for example:
      // Name1\n\x1F
      // Value1\n\x1E
      // Name2\n\x1F
      // Value2\n\x1E        
      // see the examples at http://www.temboo.com/arduino for more details
      // we can read this format into separate variables, as follows: 
      while(HomeTimelineChoreo.available()) {
        // read the name of the output item
        String name = HomeTimelineChoreo.readStringUntil('\x1F');
        name.trim();
        // read the value of the output item
        String data = HomeTimelineChoreo.readStringUntil('\x1E');
        data.trim();
        // assign the value to the appropriate String
        if (name == "tweet") {
          tweet = data;
        } else if (name == "author") {
          author = data;
        }
      }
      tft.setTextColor(ILI9340_GREEN);
      tft.setTextSize(3);
      tft.println("@" + author + " - ");
      tft.println(tweet);
      tft.println();
    } else {
      // there was an error
      // print the raw output from the choreo
      while(HomeTimelineChoreo.available()) {
        char c = HomeTimelineChoreo.read();
        tft.setTextColor(ILI9340_RED);
        tft.setTextSize(2);
        tft.println(c);
        tft.println();
      }
    }
    HomeTimelineChoreo.close();
    tft.setTextColor(ILI9340_YELLOW);
    tft.setTextSize(2);
    tft.println("Waiting...");
    delay(90000); // wait 90 seconds between Home Timeline calls
}


