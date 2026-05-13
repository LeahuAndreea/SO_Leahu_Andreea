#include "biblioteca.h"



void create(const char *district)
{
    if (mkdir(district, 0750) == -1) //exista deja
        {
        if (errno != EEXIST)
        {
            fprintf(stderr,"Eroare: districul %s exista deja, nu este necesar sa il creem\n", district);
            return;
        }
    }
    //pentru siguranta, setam inca o data modul
    chmod(district, 0750);

    char path[PATH_SIZE];
    int f;

    //reports.dat
    snprintf(path,sizeof(path),"%s/reports.dat", district);
    f=open(path, O_CREAT | O_RDWR, 0664);
    if (f == -1)
    {
        perror("Eroare la deschiderea reports.dat");
        return;
    }
    chmod(path, 0664);
    close(f);

    //district.cfg
    snprintf(path, sizeof(path), "%s/district.cfg", district);
    f=open(path,O_CREAT|O_WRONLY,0640);
    if (f == -1)
    {
        perror("Eroare la deschiderea district.cfg");
        return;
    }
    if (lseek(f, 0, SEEK_END) == 0)
    {
        char buf[] = "Threshold=1\n";
        write(f, buf, strlen(buf));
    }
    close(f);
    chmod(path, 0640);


    //logged_district
    snprintf(path, sizeof(path),"%s/logged_district",district);
    f=open(path, O_CREAT | O_RDWR, 0644);
    if (f == -1)
    {
        perror("Eroare la deschiderea logged\n");
        return;
    }
    chmod(path, 0644);
    close(f);

    //symlink
    char link_name[LINK_SIZE];
    snprintf(link_name,sizeof(link_name),"active_reports-%s",district); //numele linkului
    snprintf(path,sizeof(path),"%s/reports.dat",district); // catre cine duce linkul

    struct stat lst;
    if (lstat(link_name, &lst)==0)
    {
        //exista ceva cu numele acesta
        struct stat st;
        if (stat(link_name,&st)==-1) //danglink esuat, trebuie sa repar
        {
            printf("Warning: Dangling symlink %s\n",link_name);
            unlink(link_name); //rup legatura
            symlink(path,link_name); //refac
        }
    }
    else
    {
        symlink(path,link_name);   //nu exista deloc, ul creez
    }
}

void add_report (const char *district, const char *user)
{
    REPORT r;
    char path [PATH_SIZE];
    int f;
    //create(district);
    printf("introduceti datele corespunzatoare: \n");
    printf("x= ");
    scanf("%f",&r.gps.latitude);

    printf("y= ");
    scanf("%f",&r.gps.longitude);

    printf("Category (road/lighting/flooding/other):");
    scanf("%s",r.category);

    printf("serverity level (1/2/3) ");
    scanf("%d",&r.severity);

    printf("description ");
    char text[DESCRIPTION_SIZE];
    getchar();
    fgets(r.description,DESCRIPTION_SIZE,stdin);
    r.description[strcspn(r.description,"\n")] = 0;


    snprintf(path,sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDWR,0664);

    if (f == -1)
    {
        fprintf(stderr,"Eroare la deschiderea reports.dat\n");
        return;
    }

    struct stat st;
    fstat(f,&st);
    if (st.st_size ==0)
    {
        r.id=1; // primul raport
    }
    else
    {
        REPORT ultimul;
        lseek(f,-sizeof(REPORT),SEEK_END);
        read(f,&ultimul,sizeof(REPORT));
        r.id=ultimul.id+1;
    }

    strncpy (r.inspectorName,user,NAME_SIZE-1);
    r.timestamp=time(NULL);

    lseek (f,0,SEEK_END);
    if (write(f,&r,sizeof(REPORT)) ==-1)
    {
        perror("Error saving raport");
    }
    else
    {
        printf("succes! Raportul a fost salvat cu ID-ul %d\n",r.id);
    }
    close(f);
}

void add_logged_district(const char *district, const char *user, const char *role, const char *action)
{
    char path[PATH_SIZE];
    char log[LOG_SIZE];
    int f;

    snprintf(path,sizeof(path),"%s/logged_district",district);

    struct stat st;
    if (stat(path, &st) == -1)
    {
        perror("eroare la stat in logged_district\n");
        return;
    }
    //managerul(usr) e singurul care are voie sa scrie
    if ((st.st_mode & S_IWUSR)==0)
    {
        fprintf(stderr,"Eroare: nu are permisiune de scriere pe logged_district\n");
        return;
    }

    f=open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (f == -1)
    {
        perror("Eroare la deschiderea log-ului");
        return;
    }

    time_t now = time(NULL);
    snprintf(log,sizeof(log),"%s\t%ld\t%s\t%s\n",action,(long)now,role,user);
    write(f,log,strlen(log));
    close(f);
}

void sir_permisiuni (mode_t mode, char *perm)
{
    perm[0] = mode & S_IRUSR ? 'r' : '-';
    perm[1] = mode & S_IWUSR ? 'w' : '-';
    perm[2] = mode & S_IXUSR ? 'x' : '-';

    perm[3] = mode & S_IRGRP ? 'r' : '-';
    perm[4] = mode & S_IWGRP ? 'w' : '-';
    perm[5] = mode & S_IXGRP ? 'x' : '-';

    perm[6] = mode & S_IROTH ? 'r' : '-';
    perm[7] = mode & S_IWOTH ? 'w' : '-';
    perm[8] = mode & S_IXOTH ? 'x' : '-';

    perm[9] = '\0';
}

void list(const char *district)
{
    char path[PATH_SIZE];
    REPORT r;
    int f;

    snprintf(path, sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDONLY);
    if (f==-1)
    {
        perror("Eroare deschidere raports.dat la listare\n");
        return;
    }
    while (read(f,&r,sizeof(REPORT)) > 0)
    {
        printf("ID: %d\n", r.id);
        printf("Inspector: %s\n", r.inspectorName);
        printf("GPS: %.2f, %.2f\n", r.gps.latitude, r.gps.longitude);
        printf("Categorie: %s\n", r.category);
        printf("Severitate: %d\n", r.severity);
        printf("Timestamp: %s", ctime(&r.timestamp));
        printf("Descriere: %s\n", r.description);
        printf("-------------------\n");
    }

    struct stat st;
    if (stat(path,&st) == -1)
    {
        perror("Eroare citire atribute fisier\n");
        return;
    }

    char perm[10];
    sir_permisiuni(st.st_mode,perm);

    printf("permisiunile din fisier sunt:\n");
    printf ("Permissions: %s\n",perm);
    printf("file size: %ld byte\n",st.st_size);
    printf("Last modification: %s",ctime(&st.st_mtime));

}

void view(const char *district, int raport_id)
{
    REPORT r;
    char path[PATH_SIZE];
    int f;

    snprintf(path, sizeof(path),"%s/reports.dat",district);
    f=open(path,O_RDONLY);
    if (f==-1)
    {
        perror("Eroare deschidere raports.dat la listare\n");
        return;
    }
    int v=0;
    while (read(f,&r,sizeof(REPORT))>0)
    {
        if (r.id==raport_id)
        {
            printf("ID: %d\n",r.id);
            printf("Inspector: %s\n",r.inspectorName);
            printf("GPS: %.2f,%.2f\n",r.gps.latitude,r.gps.longitude);
            printf("Categorie: %s\n",r.category);
            printf("Severitate: %d\n",r.severity);
            printf("Timestamp: %s",ctime(&r.timestamp));
            printf("Descriere: %s\n",r.description);
            printf("-------------------\n");
            v=1;
            break;
        }
    }
    if (v==0)
        printf("Raportul cu ID-ul %d nu exista.\n",raport_id);
    close(f);
}

void update_threshold (const char *district, int value)
{
    char path [PATH_SIZE];
    snprintf(path, sizeof(path),"%s/district.cfg",district);

    struct stat st;
    if (stat(path,&st) == -1)
    {
        perror("a aparut o eroare la structura stat la fisierul district.cfg\n");
        return;
    }

    if ((st.st_mode & 0777)!=0640)
    {
        fprintf(stderr,"Eroare! permisiunile de la district.cfg au fost modificate\n");
        return;
    }

    int f;
    f=open(path,O_WRONLY | O_TRUNC, 0640);
    if (f==-1)
    {
        perror("Eroare deschidere district.cfg\n");
        return;
    }

    char buf[64];
    snprintf(buf,sizeof(buf),"Threshold=%d\n",value);
    write(f,buf,strlen(buf));
    close(f);
    printf("Threshold actualizat la %d\n",value);
}

void remove_report(const char *district,int raport_id)
{
    char path[PATH_SIZE];
    snprintf(path,sizeof(path),"%s/reports.dat",district);

    int f=open(path,O_RDWR);
    if (f==-1)
    {
        perror("Eroare la deschiderea reports.dat pentru ștergere\n");
        return;
    }

    struct stat st;
    if (fstat(f,&st) == -1)
    {
        perror("Eroare la fstat");
        close(f);
        return;
    }

    REPORT r;
    int index_de_sters=-1;
    int i=0;

    while (read(f,&r,sizeof(REPORT))>0)
    {
        if (r.id==raport_id)
        {
            index_de_sters=i;
            break;
        }
        i=i+1;
    }

    if (index_de_sters == -1)
    {
        printf("Raportul cu ID %d nu a fost găsit în districtul %s!\n", raport_id, district);
        close(f);
        return;
    }

    int total=st.st_size/sizeof(REPORT);

    for (i=index_de_sters+1;i<total;i++)
    {
        lseek(f,i*sizeof(REPORT),SEEK_SET);
        read(f,&r,sizeof(REPORT));

        lseek(f,(i - 1)*sizeof(REPORT), SEEK_SET);
        write(f,&r,sizeof(REPORT));
    }

    if (ftruncate(f,(total-1)*sizeof(REPORT))==-1)
    {
        perror("Eroare la stergere ultimul rand (cel dublificat)\n");
    }
    else
    {
        printf("Raportul cu id %d a fost sters\n",raport_id);
    }
    close(f);
}

void remove_district (const char *district)
{
    char link_name[LINK_SIZE];
    snprintf(link_name,sizeof(link_name),"active_reports-%s",district);
    unlink(link_name);

    pid_t pid=fork();
    if (pid<0)
    {
        perror("fork failed\n");
        exit(1);
    }
    if (pid==0) //cod fiu
    {
        execlp("rm","rm","-rf",district,NULL);

        perror("execlp a esuat\n");
        exit(-1);
    }
    //codparinte
    wait(NULL);
    printf("District %s removed \n",district);
}

int parse_condition(const char *input, char *field, char *op, char *value)
{
    // copiem input-ul ca sa nu il modificam
    char copy[100];
    strncpy(copy, input, sizeof(copy)-1);

    // strtok sparge string-ul dupa ":"
    char *token = strtok(copy, ":");
    if (token == NULL) return 0;
    strcpy(field, token);  // ex: "severity"

    token = strtok(NULL, ":");
    if (token == NULL) return 0;
    strcpy(op, token);     // ex: ">="

    token = strtok(NULL, ":");
    if (token == NULL) return 0;
    strcpy(value, token);  // ex: "2"

    return 1;  // succes
}

int match_condition(REPORT *r, const char *field, const char *op, const char *value) {

    if (strcmp(field, "severity") == 0) {
        int val = atoi(value);  // convertim "2" la int
        if (strcmp(op, "==") == 0) return r->severity == val;
        if (strcmp(op, "!=") == 0) return r->severity != val;
        if (strcmp(op, "<")  == 0) return r->severity <  val;
        if (strcmp(op, "<=") == 0) return r->severity <= val;
        if (strcmp(op, ">")  == 0) return r->severity >  val;
        if (strcmp(op, ">=") == 0) return r->severity >= val;
    }

    if (strcmp(field, "category") == 0) {
        int cmp = strcmp(r->category, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }

    if (strcmp(field, "inspector") == 0) {
        int cmp = strcmp(r->inspectorName, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }

    if (strcmp(field, "timestamp") == 0) {
        time_t val = (time_t)atol(value);  // timestamp e long
        if (strcmp(op, "==") == 0) return r->timestamp == val;
        if (strcmp(op, "!=") == 0) return r->timestamp != val;
        if (strcmp(op, "<")  == 0) return r->timestamp <  val;
        if (strcmp(op, "<=") == 0) return r->timestamp <= val;
        if (strcmp(op, ">")  == 0) return r->timestamp >  val;
        if (strcmp(op, ">=") == 0) return r->timestamp >= val;
    }

    return 0;  // field necunoscut
}

void filter (const char *district, int argc, char *argv[])
{
    char path[PATH_SIZE];
    snprintf(path, sizeof(path),"%s/reports.dat",district);

    int f;
    f=open(path,O_RDONLY);
    if (f==-1)
    {
        perror("eroare deschidere fisier\n");
        return;
    }

    REPORT r;
    int i;
    int v=0;
    while ( read (f,&r,sizeof(REPORT))>0)
    {
        int match=1;
        for (i=7;i<argc;i++)
        {
            char field[50];
            char op[10];
            char value [50];
            parse_condition(argv[i],field,op,value);
            if (match_condition(&r,field,op,value)==0)
            {
                match=0; //nu a fost indeplinita una dintre conditii
                break;
            }
        }
        if (match==1)
        {
            printf("ID: %d\n", r.id);
            printf("Inspector: %s\n", r.inspectorName);
            printf("GPS: %.2f, %.2f\n", r.gps.latitude, r.gps.longitude);
            printf("Categorie: %s\n", r.category);
            printf("Severitate: %d\n", r.severity);
            printf("Timestamp: %s\n", ctime(&r.timestamp));
            printf("Descriere: %s\n", r.description);
            printf("-------------------\n");
            v=v+1;
        }
    }
    if (v==0)
    {
        printf("Niciun raport nu corespunde conditiilor\n");
    }
    close(f);
}

int check_permission(const char *path, const char *role, char action)
{
    struct stat st;
    if (stat(path, &st) == -1) return 0;

    // Managerul e user, inspectorii sunt grupul
    if (strcmp(role, "manager") == 0)
    {
        if (action == 'r') return (st.st_mode & S_IRUSR);
        if (action == 'w') return (st.st_mode & S_IWUSR);
    }
    else
        if (strcmp(role, "inspector") == 0)
        {
            if (action == 'r') return (st.st_mode & S_IRGRP);
            if (action == 'w') return (st.st_mode & S_IWGRP);
        }
    return 0;
}

void notify_monitor(const char *district, const char *user, const char *role)
{
    int f;
    int monitor_informed=0;
    f=open (".monitor_pid",O_RDONLY);
    if (f!=-1)
    {
        char pid_str[10];
        int bytes=read(f,pid_str,sizeof(pid_str)-1);
        close(f);
        if (bytes>0)
        {
            pid_str[bytes]='\0';
            pid_t monitor_pid=atoi(pid_str);

            if (kill(monitor_pid,SIGUSR1)==0)
                monitor_informed=1;
        }
    }
    char text[100];
    if (monitor_informed!=0)
    {
        snprintf(text, sizeof(text),"Add report. monitor informed\n");
        printf("Monitorul a fost informat\n");
    }
    else
    {
        snprintf(text, sizeof(text),"Add report. monitor could not be informed\n");
        fprintf(stderr,"Monitorul NU a fost informat!\n");
    }
    add_logged_district(district,user,role,text);

}


int main(int argc,char *argv[]) {
    if (argc < 7)
    {
        printf("Numar necorespunzator de argumente \n");
        return 1;
    }

    char *role=argv[2];
    char *user=argv[4];
    char *comand=argv[5];
    char *district=argv[6];

    create(district);

    if (strcmp(role,"manager")!=0 &&strcmp (role,"inspector")!=0)
    {
        printf("Eroare:rolul %s este necunoscut\n",role);
        return 1;
    }

    char path_reports[PATH_SIZE];
    char path_cfg [PATH_SIZE];
    char path_log[PATH_SIZE];

    snprintf(path_reports,sizeof(path_reports),"%s/reports.dat",district);
    snprintf (path_cfg,sizeof(path_cfg),"%s/district.cfg",district);
    snprintf(path_log,sizeof(path_log),"%s/logged_district",district);

    if (strcmp(comand, "--add") == 0)
    {
        if (check_permission (path_reports, role, 'w')==0)
        {
            printf("Eroare: %s nu are drept de scriere pe raports.dat\n",user);
            return 1;
        }
        add_report(district,user);
        if (strcmp(role,"manager")==0)
        {
            notify_monitor(district,user,role);
        }
    }
    else
    {
        if (strcmp(comand,"--list")==0)
        {
            if (check_permission (path_reports, role, 'r')==0)
            {
                printf("Eroare: %s nu are dreptul de citire pe raport.dat\n",role);
                return 1;
            }
            list(district);
            if (strcmp(role,"manager")==0)
            {
                add_logged_district(district,user,role,"list");
            }
        }
        else
        {
            if (strcmp(comand,"--view") == 0)
            {
                if (argc<8)
                {
                    printf ("Erroare: este obligatorie existenta unui id pt. apelul functiei view\n");
                    return 1;
                }
                if (check_permission (path_reports, role, 'r')==0)
                {
                    printf("Eroare: %s nu are dreptul de citire pe raport.dat\n",role);
                    return 1;
                }
                int idRaport=atoi(argv[7]);
                view(district,idRaport);
                if (strcmp(role,"manager")==0)
                {
                    add_logged_district(district,user,role,"view");
                }
            }
            else
            {
                if (strcmp(comand, "--update_threshold") == 0)
                {
                    if (strcmp(role, "manager") != 0)
                    {
                        fprintf(stderr, "Eroare: doar managerul poate face update_threshold\n");
                        return 1;
                    }
                    if (argc < 8)
                    {
                        fprintf(stderr, "Eroare: trebuie specificata o valoare pentru threshold\n");
                        return 1;
                    }
                    if (check_permission(path_cfg, role, 'w') == 0)
                    {
                        fprintf(stderr, "Eroare: %s nu are drept de scriere pe district.cfg\n", user);
                        return 1;
                    }
                    int id = atoi(argv[7]);
                    update_threshold(district, id);
                    add_logged_district(district, user, role, "update_threshold");
                }
                else
                {
                    if (strcmp (comand,"--remove_report")==0)
                    {
                        if (strcmp(role, "manager") != 0)
                        {
                            fprintf(stderr, "Eroare: doar managerul poate face stergere\n");
                            return 1;
                        }
                        if (argc < 8)
                        {
                            fprintf(stderr, "Eroare: trebuie specificata un index pentru stergere report\n");
                            return 1;
                        }
                        if (check_permission(path_reports, role, 'w') == 0)
                        {
                            fprintf(stderr, "Eroare: %s nu are drept de scriere pe district.cfg\n", user);
                            return 1;
                        }
                        int id = atoi(argv[7]);
                        remove_report(district, id);
                        add_logged_district(district, user, role, "remove_report");
                    }
                    else
                    {
                        if (strcmp (comand, "--filter") == 0)
                        {
                            if (check_permission(path_reports, role, 'r') == 0)
                            {
                                fprintf(stderr, "Eroare: %s nu are drept de citire pe reports.dat\n", user);
                                return 1;
                            }
                            if (argc < 8)
                            {
                                fprintf(stderr, "Eroare: trebuie specificata cel putin o conditie pentru filter\n");
                                return 1;
                            }
                            filter(district, argc, argv);
                            if (strcmp(role, "manager") == 0)
                            {
                                add_logged_district(district, user, role, "filter");
                            }
                        }
                        else
                        {
                            if (strcmp (comand, "--remove_district") == 0)
                            {
                                if (strcmp(role, "manager") == 0)
                                {
                                    remove_district(district);
                                }
                                else
                                {
                                    printf("Eroare: %s nu are dreptul de a sterge",user);
                                }
                            }
                            else
                            {
                                printf("Eroare:Comanda %s este necunoscuta\n",comand);
                                return 1;
                            }
                        }

                    }

                }

            }
        }
    }

    return 0;
}