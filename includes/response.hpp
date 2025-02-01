class Response {
	private:
		int		clientFd;

		void	sendStartLine();
		void	sendHeaders();
		void	sendBody();
	public:
		Response();

};