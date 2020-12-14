Read() {
	switch(read_state) {
		case RInit:
			read_state = Rread;
			PA = 0;
			break;
		case Rread:
			break;
		default:
			read_state = RInit;
			break;
	}
	switch(read_state) {
		case RInit:
			PA = 0;
			break;
		case Rread:
			PA = ~PINA;
			break;
		default:
			break;
	}
}
