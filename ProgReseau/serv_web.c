#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
// pas encore terminé 

#define BUFSIZE 512
enum TypeFichier { NORMAL, REPERTOIRE, ERREUR };

const char* OK200 = "HTTP/1.1 200 OK\r\n\r\n";
const char* ERROR403 = "HTTP/1.1 403 Forbidden\r\n\r\nAccess denied\r\n";
const char* ERROR404 = "HTTP/1.1 404 Not Found\r\n\r\nFile or directory not found\r\n";


/* Fonction typeFichier()
 * argument: le nom du fichier
 * rend une valeur de type enumeration declaree en tete du fichier
 * NORMAL, ou REPERTOIRE ou ERRREUR
 */
enum TypeFichier typeFichier(char *fichier) {
  struct stat status;
  int r;

  r = stat(fichier, &status);
  if (r < 0)
    return ERREUR;
  if (S_ISREG(status.st_mode))
    return NORMAL;
  if (S_ISDIR(status.st_mode))
    return REPERTOIRE;
  /* si autre type, on envoie ERREUR (a fixer plus tard) */
  return ERREUR;
}


/* envoiFichier()
 * Arguments: le nom du fichier, la socket
 * valeur renvoyee: true si OK, false si erreur
 */
#define BUSIZE 1048;
bool envoiFichier(char *fichier, int soc) {
  int fd,i;
  char buf[BUFSIZE];
  ssize_t nread;

  /* A completer.
   * On peut se poser la question de savoir si le fichier est
   * accessible avec l'appel systeme access();
   * Si oui, envoie l'entete OK 200 puis le contenu du fichier
   * Si non, envoie l'entete ERROR 403
   *
   * Note: le fichier peut etre plus gros que votre buffer,
   * de meme il peut etre plus petit...
   */

	if(access(fichier,R_OK)!=0){
		printf("le fichier n'est pas accessible\n");
		write(soc, ERROR403, strlen(ERROR403));
		return false;		
		}
	
	else {
	 fd = open(fichier,O_RDONLY);
  	write(soc,OK200,strlen(OK200));

//boucle
	while((nread != 0) && (nread != -1)){
	 nread= read(fd,buf,BUFSIZE);
	 write(soc,buf,nread);
}
	
	return true;

}

}


/* envoiRep()
 * Arguments: le nom du repertoire, la socket
 * valeur renvoyee: true si OK, false si erreur
 */
bool envoiRep(char *rep, int soc) {
  DIR *dp;
  struct dirent *pdi;
  char buf[1024], nom[1024];

  dp = opendir(rep);
  if (dp == NULL)
    return false;

  write(soc, OK200, strlen(OK200));
  sprintf(buf, "<html><title>Repertoire %s</title><body>", rep);
  write(soc, buf, strlen(buf));

  while ((pdi = readdir(dp)) != NULL) {
    /* A completer
     * Le nom de chaque element contenu dans le repertoire est retrouvable a
     * l'adresse pdi->d_name. Faites man readdir pour vous en convaincre.
     * Dans un premier temps, on se contentera de la liste simple.
     * Dans une petite amelioration on poura prefixer chaque element avec
     * l'icone folder ou generic en fonction du type du fichier.
     * (Tester le nom de l'element avec le chemin complet.) */
		
			strcpy(nom,pdi->d_name);
			if(nom == "." || nom == "..")
				continue;
 
			write(soc,nom,strlen(nom));
		
			








 }
  write(soc, "\n\r", 2);
  return true;
}


void communication(int ns, struct sockaddr_storage from, socklen_t fromlen) {
  int s;
  char buf[BUFSIZE];
  ssize_t nread, nwrite;
  char host[NI_MAXHOST];
  bool result;
  char *pf;
  enum op { GET, PUT } operation;

  /* Eventuellement, inserer ici le code pour la reconnaissance de la
   * machine appelante */
	
	s = getnameinfo((struct sockaddr *)&from, fromlen,
			host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
  if (s == 0)
    printf("Debut avec client '%s'\n", host);
  else
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

  for (;;) {
   /* nwrite = write(ns, pf, strlen(message));
    if (nwrite < 0) {
			perror("write");
			close(ns);
			break;
		}*/
		nread = read(ns, buf, BUFSIZE);
		if (nread == 0) {
			printf("Fin avec client '%s'\n", host);
			close(ns);
			break;
		} else if (nread < 0) {
			perror("read");
			close(ns);
			break;
    }
    buf[nread] = '\0';
    printf("Message recu '%s'\n", buf);
  }


  /* Reconnaissance de la requete */
  nread = read(ns, buf, BUFSIZE);
  if (nread > 0) {
    if (strncmp(buf, "GET", 3) == 0)
      operation = GET;
  } else {
    perror("Erreur lecture socket");
    return;
  }

  switch (operation) {
    case GET:
      pf = strtok(buf + 4, " ");
      /* On pointe alors sur le / de "GET /xxx HTTP...
       * strtok() rend l'adresse du premier caractere
       * apres l'espace et termine le mot par '\0'
       */
      pf++; /* pour pointer apres le slash */
      /* pf pointe sur le nom du fichier suivant le / de la requete.
       * Si la requete etait "GET /index.html ...", alors pf pointe sur
       * le "i" de "index.html"
       */
      /* si le fichier est un fichier ordinaire, on l'envoie avec la fonction
       * envoiFichier().
       * Si c'est un repertoire, on envoie son listing avec la fonction
       * envoiRep().
       * Vous pouvez utiliser la fonction typeFichier() ci-dessous pour tester
       * le type du fichier.
       */

      /************ A completer ici**********/
			
			switch(typeFichier(pf))
				{
			case NORMAL:
			if(!envoiFichier(pf,ns))
		printf("impossible d'envoyer le fichier\n");
			break;			
			case REPERTOIRE:
			if(!envoiRep(pf,ns))
		printf("impossible d'envoyer le listing du repertoire\n");
		break;	
			default:
		printf("erreur\n");			
		break;	
  }

  close(ns);
}


int main(int argc, char **argv) {
  int sfd, s, ns, r, pid;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  struct sockaddr_storage from;
  socklen_t fromlen;

  if (argc != 2) {
    printf("Usage: %s  port_serveur\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Inserer ici le code d'un serveur TCP concurent */

  /* Construction de l'adresse locale (pour bind) */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;           /* Force IPv6 */
  hints.ai_socktype = SOCK_STREAM;      /* Stream socket */
  hints.ai_flags = AI_PASSIVE;          /* Adresse IP joker */
  hints.ai_flags |= AI_V4MAPPED|AI_ALL; /* IPv4 remapped en IPv6 */
  hints.ai_protocol = 0;                /* Any protocol */

  s = getaddrinfo(NULL, argv[1], &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  /* getaddrinfo() retourne une liste de structures d'adresses.
     On essaie chaque adresse jusqu'a ce que bind(2) reussisse.
     Si socket(2) (ou bind(2)) echoue, on (ferme la socket et on)
     essaie l'adresse suivante. cf man getaddrinfo(3) */
  for (rp = result; rp != NULL; rp = rp->ai_next) {

    /* Creation de la socket */
    sfd = socket(rp->ai_family,rp->ai_socktype,0);
    if (sfd == -1)
      continue;

    /* Association d'un port a la socket */
	 
    r = bind(sfd,rp->ai_addr,rp->ai_addrlen);
    if (r == 0)
      break;            /* Succes */
    close(sfd);
  }

  if (rp == NULL) {     /* Aucune adresse valide */
    perror("bind");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(result); /* Plus besoin */

  /* Positionnement de la machine a etats TCP sur listen */
  listen(sfd,5);

  for (;;) {
    /* Acceptation de connexions */
    fromlen = sizeof(from);
    ns = accept(sfd,(struct sockaddr *)&from,&fromlen);
    if (ns == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
  	}

		pid=fork(); /* fork */



		if(pid!=0){ /* on est dans le pere */
			close(ns);
			signal(SIGCLD, SIG_IGN); 
		}


		else { /* on est dans le fils */
			close(sfd);
		
			printf("on est dans le fils\n");
			communication(ns,from ,fromlen);		
			exit(EXIT_SUCCESS);		
		}
	}
}
}
