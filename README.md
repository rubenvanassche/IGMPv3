# Telecom
Project voor Telecommunicatiesystemen.

## Hoe compileren?
>> make

Compileert en runt ipnetwork.click

## Hoe gebruiken?
Elke client heeft een IGMPClient, hierbij zijn twee handlers:
- include_sources voor een include
- exclude_sources voor een exclude

Deze moeten volgend argument bevatten: M {multicast_address}, S 1.1.1.1
(S was voor een uitbreiding die helaas niet geprogrammeerd is).

Met deze handlers kunnen clients deelnemen aan de multicast of afhaken.

## Wat werkt niet?
- General query's worden verstuurd maar niet verwerkt
- Group Specific query's worden niet verstuurd
- Er wordt geen rekening gehouden met timers
- Stukken van het protocol zijn niet zo goed geïmplenteerd(bijvoorbeeld samengestede raporten)

## Wat werkt wel?
- Een client kan joinen of leaven
- Pakketten zijn correct volgens wireshark(query en report)
- General queries worden om de 10 seconden verstuurd
