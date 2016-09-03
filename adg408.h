#ifndef ADG408_H
#define ADG408_H

#include <stdint.h>

class ADG408 {
    public:

        //--------------------------------------------------------------------------------------------------------------
        // Constructor
        //--------------------------------------------------------------------------------------------------------------
        ADG408 (uint8_t en_pin, uint8_t a0_pin, uint8_t a1_pin, uint8_t a2_pin);

        //--------------------------------------------------------------------------------------------------------------
        // Public Functions
        //--------------------------------------------------------------------------------------------------------------
        void selectChannel (uint8_t ichan);
        void enable();
        void disable();

    private:

        //--------------------------------------------------------------------------------------------------------------
        // Configuration
        //--------------------------------------------------------------------------------------------------------------

        struct Config {
            uint8_t en_pin;
            uint8_t a0_pin;
            uint8_t a1_pin;
            uint8_t a2_pin;
        } config;

};
#endif /* ADG408_H */
