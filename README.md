# Telecom
Project voor Telecommunicatiesystemen.

## Hoe compileren?
> make configure
> make

Compileert en runt ipnetwork.click

## Hoe gebruiken?
Elke client heeft een IGMPClient, hierbij zijn twee handlers:
- include_sources voor een include
- exclude_sources voor een exclude

Deze moeten volgend argument bevatten: M {multicast_address}, S 1.1.1.1
(S was voor een uitbreiding die helaas niet geprogrammeerd is).

Met deze handlers kunnen clients deelnemen aan de multicast of afhaken.

