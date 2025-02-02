FROM debian:bullseye

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y build-essential wget curl vim zsh git lsof net-tools siege && \
    apt-get clean
RUN sh -c "$(wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O -)"

EXPOSE 4242 1919 2121

CMD ["zsh"]