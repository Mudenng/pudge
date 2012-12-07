#include <tcutil.h>
#include <tchdb.h>
#include <stdio.h>

int main() {
	TCHDB *hdb;
	char *key, *value;

	hdb = tchdbnew();
	tchdbopen(hdb, "db.hdb", HDBOWRITER | HDBOCREAT);
	tchdbput2(hdb, "one", "yes");
	
	value = tchdbget2(hdb, "one");
	printf("%s\n", value);

	return 0;
}
