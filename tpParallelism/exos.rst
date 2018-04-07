Exercices de parallélismes
==========================

Min/Max
-------

Écrivez une fonction qui calcule le plus petit *et* le plus grand élément d'un
tableau de nombres *flottants* passez en paramètres.

.. code:: c

    #include <stdlib.h>
    #include <stdio.h>
    #include <limits.h>
    #include <sys/time.h>

    static void minmax(float const* start, float const* stop, float* min, float* max) {
      // giveme giveme a man fmin
    }

    int main(int argc, char**argv) {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        float* data = malloc(n * sizeof(*data));
        for(int i = 0; i < n; ++i)
            data[i] = (unsigned)i * (INT_MAX / 3); // pourquoi ce cast?

        struct timeval start, stop;
        float min, max;
        gettimeofday(&start, NULL);
        minmax(data, data +n, &min, &max);
        gettimeofday(&stop, NULL);
        printf("%f - %f\n", min, max);
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }

Inspectez le code généré: utilise-t-il des instructions vectorielles ? Si non,
en vous basant sur le jeu d'instruction disponible sur votre machine (``car
/proc/cpuinfo``), l'[aide en
ligne](https://software.intel.com/sites/landingpage/IntrinsicsGuide/) et votre
cervelet droit, implémentez en une!

C'est la règle
==============

Le programme suivant affichez une forme de règle graduée sur votre écran :

.. code:: c

    #include <stdlib.h>
    #include <stdio.h>
    #include <stddef.h>
    #include <sys/time.h>

    static void rule_incr(int* pos, int level) {
      *pos = level;
    }

    static int ilog(unsigned val) {
      int n = 0;
      while(val)
      {
        n += 1;
        val >>=2;
      }
      return n;
    }

    static void rule_aux(int* start, int* stop, int level) {
      if(level==0) return;
      ptrdiff_t n = stop - start;
      rule_incr(start + n/2, level);
      rule_aux(start, start + n/2, level - 1);
      rule_aux(start + n/2, stop, level - 1);
    }

    static void rule(int* start, int* stop) {
      rule_aux(start, stop, 1 + ilog(stop - start));
    }

    int main(int argc, char**argv) {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        // is that the best data type?
        int* data = calloc(n, sizeof(*data));

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        rule(data, data + n);
        gettimeofday(&stop, NULL);
        for(int j = 0; j < n; ++j) {
          for(int k = 0; k < data[j]; ++k)
            putchar('#');
          putchar('\n');
        }
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }

Faites en sorte que la construction de la règle se fasse en parallèle !

Prenez aussi le temps de regarder le code assembleur généré pour les appels récursifs… Que s'est-il passé ?

Lock (Lamora) less Data structure
=================================

Le programme suivant implémente une structure de donnée qui n'est pas *thread-safe*.

.. code:: c

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>

    typedef struct list {
      struct list* next;
      int val;
    } * list_t;

    list_t const empty_list = NULL;

    list_t make_list(int val) {
      list_t l = malloc(sizeof(*l));
      l->val = val;
      l->next = empty_list;
      return l;
    }

    void list_push_front(list_t *self, int val) {
      list_t res = make_list(val);
      res->next = *self;
      *self = res;
    }

    int list_pop_front(list_t* self) {
      assert(*self && "pop from empty list o_O");
      int val = (*self)->val;
      list_t next = (*self)->next;
      free(*self);
      *self = next;
      return val;
    }

    int list_front(list_t self) {
      assert(self && "front from empty list O_o");
      return self->val;
    }

    size_t list_size(list_t self) {
      size_t n = 0;
      while(self) {
        n += 1;
        self = self->next;
      }
      return n;
    }

    static size_t process(list_t *self, int add_count, int rm_count) {
      while(add_count--)
        list_push_front(self, add_count);
      while(rm_count--)
        list_pop_front(self);
      return list_size(*self);
    }

    int main(int argc, char** argv) {
      if(argc != 3) return 1;

      int add_count = atoi(argv[1]);
      int rm_count = atoi(argv[2]);
      list_t curr = empty_list;
      size_t final_size = process(&curr, add_count, rm_count);
      printf("%zd\n", final_size);
      return 0;

    }

Modifiez le en utilisant des [opérations
atomiques](http://en.cppreference.com/w/c/language/atomic) afin que la
structure soit *thread-safe*. Pour tester l'ensemble, vous pourrez utiliser un
appel à ``pthread_create`` pour faire plusieurs appels à ``process``.

Extraction de pages
===================

Que fait le script suivant ?

.. code::sh

    while read line
    do
        wget -O - "$line"
    done 2>/dev/null | grep -E -o 'href="http://[^"]*"' | sed -r 's/href="(.*)"/\1/'

Il es amusant (si si) de constater que la sortie peut être envoyer sur l'entrée. En procédant ainsi une fois, combien de pages aura-t-on visité ?

Comment accélérer ce traitement en utilisant l'opérateur ``&`` et la commande ``wait`` du shell ? Et en utilisant le programme GNU ``parallel`` ou ``xargs``?

Quel problème d'ordonnancement est illustré par ce programme ?


Tri parallèle
-------------

En réutilisant la fonction ``qsort`` et à l'aide de directives OpenMP, écrivez
une version parallèles d'un algorithme de tri de nombres entiers. La fonction ``bsearch(3)`` trouvera ici son utilité :-)

Comparez ensuite vos résultats en utilisant des tableaux d'entiers de taille
différente.  Comment évolue le nombre d'octets traités par seconde ?


.. code:: c

    #include <stdlib.h>
    #include <stdio.h>
    #include <limits.h>
    #include <sys/time.h>

    static int cmp(int const* self, int const* other) {
        return self - other;
    }

    int main(int argc, char**argv) {
        if(argc != 2) return 1;
        int n = atoi(argv[1]);
        int* data = malloc(n * sizeof(*data));
        for(int i = 0; i < n; ++i)
            data[i] = (unsigned)i * (INT_MAX / 3);

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        qsort(data, n, sizeof(*data), (int (*)(void const*, void const*))cmp);
        gettimeofday(&stop, NULL);
        volatile __attribute__((unused)) int anchor = data[n/2]; // why that?
        printf("%lf ms\n", (stop.tv_sec - start.tv_sec) * 1000. + (stop.tv_usec - start.tv_usec) / 1000.);
        return 0;
    }


