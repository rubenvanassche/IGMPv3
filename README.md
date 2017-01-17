# Telecom
Project voor Telecommunicatiesystemen.

## Hoe compileren?
> make

Compileert en runt ipnetwork.click

## Hoe gebruiken?
Elke client heeft een IGMPClient, hierbij zijn twee handlers:
- include_sources voor een include
- exclude_sources voor een exclude

Deze moeten volgend argument bevatten: M {multicast_address}, S 1.1.1.1
(S was voor een uitbreiding die helaas niet geprogrammeerd is).

Met deze handlers kunnen clients deelnemen aan de multicast of afhaken.

## Wat werkt niet?
- Er wordt bijna geen rekening gehouden met timers
- Stukken van het protocol zijn niet zo goed geïmplenteerd(bijvoorbeeld samengestede raporten)
- Deel van vesrturen van rapporten werkt maar volgens mij niet correct volgens RFC

## Wat werkt wel?
- Een client kan joinen of leaven
- Pakketten zijn correct volgens wireshark(query en report)
- General queries worden om de 10 seconden verstuurd
