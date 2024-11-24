# Temat 17 – Salon fryzjerski

Zadanie stanowi rozszerzenie problemu **śpiącego fryzjera/golibrody** na wielu fryzjerów:  
[Problem śpiącego golibrody - Wikipedia](https://pl.wikipedia.org/wiki/Problem_śpiącego_golibrody)

## Opis problemu

W salonie pracuje **F** fryzjerów (**F > 1**) i znajduje się w nim **N** foteli (**N < F**). Salon jest czynny w godzinach od **Tp** do **Tk**. Klienci przychodzą do salonu w losowych momentach czasu. W salonie znajduje się poczekalnia, która może pomieścić **K** klientów jednocześnie. 

Każdy klient rozlicza usługę z fryzjerem przekazując mu kwotę za usługę przed rozpoczęciem strzyżenia. Fryzjer wydaje resztę po zakończeniu obsługi klienta, a w przypadku braku możliwości wydania reszty, klient musi zaczekać, aż fryzjer znajdzie resztę w kasie. Kasa jest wspólna dla wszystkich fryzjerów. Płatność może być dokonana banknotami o nominałach **10 zł**, **20 zł** i **50 zł**.

### Zasada działania fryzjera (cyklicznie):
1. Wybiera klienta z poczekalni (ewentualnie czeka, jeśli go jeszcze nie ma).
2. Znajduje wolny fotel.
3. Pobiera opłatę za usługę i umieszcza ją we wspólnej kasie (opłata może być również przekazana bezpośrednio przez klienta, ale fryzjer musi znać kwotę, żeby wyliczyć resztę do wydania).
4. Realizuje usługę.
5. Zwalnia fotel.
6. Wylicza resztę i pobiera ją ze wspólnej kasy. Jeśli nie jest to możliwe, czeka, aż pojawią się odpowiednie nominały w wyniku pracy innego fryzjera.
7. Przekazuje resztę klientowi.

### Zasada działania klienta (cyklicznie):
1. Zarabia pieniądze.
2. Przyjeżdża do salonu fryzjerskiego.
3. Jeśli jest wolne miejsce w poczekalni, siada i czeka na obsługę (ewentualnie budzi fryzjera). W przypadku braku miejsc opuszcza salon i wraca do zarabiania pieniędzy.
4. Po znalezieniu fryzjera płaci za usługę.
5. Czeka na zakończenie usługi.
6. Czeka na resztę.
7. Opuszcza salon i wraca do zarabiania pieniędzy.

### Polecenia kierownika:
1. **Sygnał 1** - Dany fryzjer kończy pracę przed zamknięciem salonu.
2. **Sygnał 2** - Wszyscy klienci (ci którzy siedzą na fotelach i z poczekalni) natychmiast opuszczają salon.

## Zadanie

Napisz procedury:
- **Kasjer (kierownik)** – zarządza pracą fryzjerów i klientami.
- **Fryzjer** – symuluje działania fryzjera.
- **Klient** – symuluje działania klienta.

Procedury powinny symulować działanie salonu fryzjerskiego.
