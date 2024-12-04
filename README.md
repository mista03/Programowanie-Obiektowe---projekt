# System Zarządzania Hotelem

## Opis klas

### 1. **Hotel**

### 2. **Room**

### 3. **Guest**

### 4. **Reservation**

### 5. **Payment** *(Abstrakcyjna)*
   Abstrakcyjna klasa bazowa reprezentująca płatność. Klasa wykorzystuje **polimorfizm dynamiczny**, umożliwiając wywoływanie odpowiednich metod płatności w zależności od typu obiektu (`CardPayment` lub `CashPayment`).

### 6. **CardPayment** *(Dziedziczy po Payment)*
   Klasa przetwarzająca płatność kartą.

### 7. **CashPayment** *(Dziedziczy po Payment)*
   Klasa przetwarzająca płatność gotówką.
