1) Ce instrument am folosit?
    1) Am utilizat Claude (claude.ai)
2) Prompt-uri date?
    1) I-am trimis cerinta problemei, ca sa aiba un context mai bun
       asupra a ceea ce trebuie sa faca. Ulterior i-am trimis codul
       pe care il scrisesem pana in acel punct si l-am rugat sa implementeze cele 2 functii:
       parse_condition si match_condition
3) Ce a fost generat?
    1) A generat funcțiile necesare și mi-a explicat pe scurt ce fac.
       Unde a fost necesar, mi-a oferit comentarii pentru fiecare linie.
        1) Funcția parse_condition împarte șirul în cele trei componente folosind
           strtok și le salvează separat;
           dacă lipsește ceva, returnează 0.
        2) Funcția match_condition verifică dacă o condiție e îndeplinită: pentru
           câmpuri numerice face conversii și compară valorile, iar pentru șiruri
           folosește strcmp; dacă apare un câmp necunoscut, returnează 0 și afișează un avertisment.
4) Ce am modificat?
    1) Am corectat faptul că interpreta greșit parametrii
       (credea că se transmite inspector name, deși aceștia sunt fixați).
       În plus, la parse_condition am adăugat terminatorul \0
       după strncpy pentru siguranță
5) Ce am invatat?
    1) E necesar sa verific ce genereaza inteligenta artificila,
       pot aparea erori