################################################################
# LOGGER
################################################################

def init_logger():
    import logging
    logger = logging.getLogger("trustify")
    handler = logging.StreamHandler()
    formatter = logging.Formatter(fmt="%(asctime)s %(levelname)-8s %(message)s", datefmt="%H:%M:%S")
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    return logger
logger = init_logger()
