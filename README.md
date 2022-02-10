# esp-piggy-bank
Un progetto per implementare un salvadanaio intelligente con alla base un ESP8266

### Componenti utilizzate

- ESP8266 D1 mini
- Arduino UNO
- Potenziometro
- ULN2003 Stepper Motor Driver Module
- Stepper Motor
- Power Module (Power MB-V2)
- 4 Digit 7-Segment Display (module 5641AS)

### Obbiettivo

L'obbiettivo di questo progetto è quello di creare un salvadanaio intelligente (smart piggy bank) che sia in grado di riconoscere il tipo di moneta inserita e che, una volta riconosciuta la moneta, invii una notifica ad un server mqtt e salvi la situazione corrente del salvadanaio nella memoria EEPROM. Per dare un feedback all'utente che inserisce la moneta è predente anche un display che comunicare l'avvenuto riconoscimento della moneta mostrando il valore della moneta stessa.

### Riconoscimento della moneta

Il riconoscimento della moneta si basa su una leva ancorata ad una estremità che viene fatta ruotare su un perno dal passaggio della moneta. Il movimento rotatorio passa quindi ad alcuni ingranaggio che amplificano il movimento rotatorio prodotto dalla leva e rendono più semplice al potenziometro distinguere le varie monete in base alla grandezza.
Una volta che la moneta riesce a passare tra la leva e la sponda rigida del salvadanaio la moneta viene identificata e la leva viene riposizionata dallo stepper motor che agisce sugli ingranaggi sottostanti alla leva.
