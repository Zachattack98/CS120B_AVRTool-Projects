Output() {
	switch(output_state) {
		case OInit:
			output_state = Ooutput;
			PORTB = 0x00;
			break;
		case Ooutput:
			break;
		default:
			output_state = OInit;
			break;
	}
	switch(output_state) {
		case OInit:
			PORTB = 0x00;
			break;
		case Ooutput:
			PORTB = PA;
			break;
		default:
			break;
	}
}
