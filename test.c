#include HEADER

int main(void) {
	const char *msg = "hello, world\n";
	char buf[20];

	strcpy(buf, msg);
	strchr(buf, ' ')[0] = '\0';
	strchr(msg, ' ')[0] = '\0';
	strchr(10,20);
}
